#pragma once

#include "video.h"
#include <kernel/heap.h>
#include <vector>
#include <mutex>

#define UNLEASHED_RECOMP

struct Profiler
{
    static constexpr size_t VALUE_COUNT = 256;

    std::atomic<double> value;
    double values[VALUE_COUNT];
    std::chrono::steady_clock::time_point start;

    void Begin()
    {
        start = std::chrono::steady_clock::now();
    }

    void End()
    {
        value = std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - start).count();
    }

    void Set(double v)
    {
        value = v;
    }

    void Reset()
    {
        End();
        Begin();
    }

    double UpdateAndReturnAverage();
};

extern size_t g_profilerValueIndex;

inline double Profiler::UpdateAndReturnAverage()
{
    values[g_profilerValueIndex] = value;
    return std::accumulate(values, values + Profiler::VALUE_COUNT, 0.0) / Profiler::VALUE_COUNT;
}

struct UploadBuffer
{
    static constexpr size_t SIZE = 16 * 1024 * 1024;

    std::unique_ptr<plume::RenderBuffer> buffer;
    uint8_t* memory = nullptr;
    uint64_t deviceAddress = 0;
};

// Forward declare RenderCommand for UploadAllocator template usage if needed? No.
extern std::unique_ptr<plume::RenderDevice> g_device;

struct UploadAllocator
{
    std::vector<UploadBuffer> buffers;
    uint32_t index = 0;
    uint32_t offset = 0;

    struct UploadAllocation
    {
        const plume::RenderBuffer* buffer;
        uint64_t offset;
        uint8_t* memory;
        uint64_t deviceAddress;
    };

    UploadAllocation allocate(uint32_t size, uint32_t alignment)
    {
        assert(size <= UploadBuffer::SIZE);

        offset = (offset + alignment - 1) & ~(alignment - 1);

        if (offset + size > UploadBuffer::SIZE)
        {
            ++index;
            offset = 0;
        }

        if (buffers.size() <= index)
            buffers.resize(index + 1);

        auto& buffer = buffers[index];
        if (buffer.buffer == nullptr)
        {
            buffer.buffer = g_device->createBuffer(plume::RenderBufferDesc::UploadBuffer(UploadBuffer::SIZE, plume::RenderBufferFlag::CONSTANT | plume::RenderBufferFlag::VERTEX | plume::RenderBufferFlag::INDEX));
            buffer.memory = reinterpret_cast<uint8_t*>(buffer.buffer->map());
            buffer.deviceAddress = buffer.buffer->getDeviceAddress();
        }

        auto ref = buffer.buffer->at(offset);
        offset += size;

        return { ref.ref, ref.offset, buffer.memory + ref.offset, buffer.deviceAddress + ref.offset };
    }

    template<bool TByteSwap, typename T>
    UploadAllocation allocate(const T* memory, uint32_t size, uint32_t alignment)
    {
        auto result = allocate(size, alignment);

        if constexpr (TByteSwap)
        {
            auto destination = reinterpret_cast<T*>(result.memory);

            for (size_t i = 0; i < size; i += sizeof(T))
            {
                *destination = ByteSwap(*memory);
                ++destination;
                ++memory;
            }
        }
        else
        {
            memcpy(result.memory, memory, size);
        }

        return result;
    }

    void reset()
    {
        index = 0;
        offset = 0;
    }
};

using UploadAllocation = UploadAllocator::UploadAllocation;

struct TextureDescriptorAllocator
{
    Mutex mutex;
    uint32_t capacity = 0; // Was TEXTURE_DESCRIPTOR_NULL_COUNT in video.cpp which is an enum value.
    // enum was:
    // TEXTURE_DESCRIPTOR_NULL_TEXTURE_2D,
    // TEXTURE_DESCRIPTOR_NULL_TEXTURE_3D,
    // TEXTURE_DESCRIPTOR_NULL_TEXTURE_CUBE,
    // TEXTURE_DESCRIPTOR_NULL_COUNT
    // I need to move this enum too or redefine it.
    static constexpr uint32_t NULL_COUNT = 3;

    std::vector<uint32_t> freed;

    uint32_t allocate()
    {
        std::lock_guard lock(mutex);

        uint32_t value;
        if (!freed.empty())
        {
            value = freed.back();
            freed.pop_back();
        }
        else
        {
            // Initial capacity was set to NULL_COUNT in video.cpp?
            // In video.cpp: uint32_t capacity = TEXTURE_DESCRIPTOR_NULL_COUNT;
            // I should make sure it starts correctly.
            // But since this struct is instantiated as global, I can set default value.
            if (capacity == 0) capacity = NULL_COUNT;

            value = capacity;
            ++capacity;
        }

        return value;
    }

    void free(uint32_t value)
    {
        assert(value != 0); // NULL check
        std::lock_guard lock(mutex);
        freed.push_back(value);
    }
};

enum
{
    TEXTURE_DESCRIPTOR_NULL_TEXTURE_2D,
    TEXTURE_DESCRIPTOR_NULL_TEXTURE_3D,
    TEXTURE_DESCRIPTOR_NULL_TEXTURE_CUBE,
    TEXTURE_DESCRIPTOR_NULL_COUNT
};

enum class CsdFilterState
{
    Unknown,
    On,
    Off
};

enum class RenderCommandType
{
    SetRenderState,
    DestructResource,
    UnlockTextureRect,
    UnlockBuffer16,
    UnlockBuffer32,
    DrawImGui,
    ExecuteCommandList,
    BeginCommandList,
    StretchRect,
    SetRenderTarget,
    SetDepthStencilSurface,
    ExecutePendingStretchRectCommands,
    Clear,
    SetViewport,
    SetTexture,
    SetScissorRect,
    SetSamplerState,
    SetBooleans,
    SetVertexShaderConstants,
    SetPixelShaderConstants,
    AddPipeline,
    DrawPrimitive,
    DrawIndexedPrimitive,
    DrawPrimitiveUP,
    SetVertexDeclaration,
    SetVertexShader,
    SetStreamSource,
    SetIndices,
    SetPixelShader,
    ExecuteLambda,
};

struct RenderCommand
{
    RenderCommandType type;
    union
    {
        struct
        {
            void* lambdaPtr;
            void (*executor)(void*);
            void (*deleter)(void*);
        } executeLambda;

        struct
        {
            GuestRenderState type;
            uint32_t value;
        } setRenderState;

        struct
        {
            GuestResource* resource;
        } destructResource;

        struct
        {
            GuestTexture* texture;
        } unlockTextureRect;

        struct
        {
            GuestBuffer* buffer;
        } unlockBuffer;

        struct
        {
            GuestDevice* device;
            uint32_t flags;
            GuestTexture* texture;
        } stretchRect;

        struct
        {
            GuestSurface* renderTarget;
        } setRenderTarget;

        struct
        {
            GuestSurface* depthStencil;
        } setDepthStencilSurface;

        struct
        {
            uint32_t flags;
            float color[4];
            float z;
        } clear;

        struct
        {
            float x;
            float y;
            float width;
            float height;
            float minDepth;
            float maxDepth;
        } setViewport;

        struct
        {
            uint32_t index;
            GuestTexture* texture;
        } setTexture;

        struct
        {
            int32_t left;
            int32_t top;
            int32_t right;
            int32_t bottom;
        } setScissorRect;

        struct
        {
            uint32_t index;
            uint32_t data0;
            uint32_t data3;
            uint32_t data5;
        } setSamplerState;

        struct
        {
            uint32_t booleans;
        } setBooleans;

        struct
        {
            uint8_t* memory;
            uint32_t index;
            uint32_t size;
        } setVertexShaderConstants;

        struct
        {
            uint8_t* memory;
            uint32_t index;
            uint32_t size;
        } setPixelShaderConstants;

        struct
        {
            XXH64_hash_t hash;
            plume::RenderPipeline* pipeline;
        } addPipeline;

        struct
        {
            uint32_t primitiveType;
            uint32_t startVertex;
            uint32_t primitiveCount;
        } drawPrimitive;

        struct
        {
            uint32_t primitiveType;
            int32_t baseVertexIndex;
            uint32_t startIndex;
            uint32_t primCount;
        } drawIndexedPrimitive;

        struct
        {
            uint32_t primitiveType;
            uint32_t primitiveCount;
            uint8_t* vertexStreamZeroData;
            uint32_t vertexStreamZeroSize;
            uint32_t vertexStreamZeroStride;
            CsdFilterState csdFilterState;
        } drawPrimitiveUP;

        struct
        {
            GuestVertexDeclaration* vertexDeclaration;
        } setVertexDeclaration;

        struct
        {
            GuestShader* shader;
        } setVertexShader;

        struct
        {
            uint32_t index;
            GuestBuffer* buffer;
            uint32_t offset;
            uint32_t stride;
        } setStreamSource;

        struct
        {
            GuestBuffer* buffer;
        } setIndices;

        struct
        {
            GuestShader* shader;
        } setPixelShader;
    };
};

// Globals
static constexpr size_t NUM_FRAMES = 2;

extern uint32_t g_frame;
extern std::unique_ptr<plume::RenderCommandList> g_commandLists[NUM_FRAMES];
extern std::unique_ptr<plume::RenderDescriptorSet> g_textureDescriptorSet;
extern std::unique_ptr<plume::RenderDescriptorSet> g_samplerDescriptorSet;
extern UploadAllocator g_uploadAllocators[NUM_FRAMES];
extern GuestSurface* g_backBuffer;
extern std::unique_ptr<plume::RenderSwapChain> g_swapChain;
extern bool g_swapChainValid;
extern moodycamel::BlockingConcurrentQueue<RenderCommand> g_renderQueue;
extern TextureDescriptorAllocator g_textureDescriptorAllocator;

extern Profiler g_gpuFrameProfiler;
extern Profiler g_presentProfiler;
extern Profiler g_updateDirectorProfiler;
extern Profiler g_renderDirectorProfiler;
extern Profiler g_frameFenceProfiler;
extern Profiler g_presentWaitProfiler;
extern Profiler g_swapChainAcquireProfiler;

extern bool g_profilerVisible;
extern bool g_profilerWasToggled;

extern plume::RenderDeviceCapabilities g_capabilities;
extern std::unique_ptr<plume::RenderInterface> g_interface;
extern uint32_t g_waitForGPUCount;
extern std::atomic<uint32_t> g_bufferUploadCount;
extern bool g_triangleStripWorkaround;
extern bool g_hardwareResolve;
extern bool g_hardwareDepthResolve;
extern std::unique_ptr<plume::RenderPipelineLayout> g_pipelineLayout;
extern std::unique_ptr<plume::RenderShader> g_copyShader;
extern std::unique_ptr<plume::RenderShader> g_copyColorShader;
extern std::unique_ptr<plume::RenderTexture> g_blankTextures[TEXTURE_DESCRIPTOR_NULL_COUNT];

extern Mutex g_copyMutex;
extern std::unique_ptr<plume::RenderCommandQueue> g_copyQueue;
extern std::unique_ptr<plume::RenderCommandList> g_copyCommandList;
extern std::unique_ptr<plume::RenderCommandFence> g_copyCommandFence;

#ifdef ASYNC_PSO_DEBUG
extern std::atomic<uint32_t> g_pipelinesCreatedInRenderThread;
extern std::atomic<uint32_t> g_pipelinesCreatedAsynchronously;
extern std::atomic<uint32_t> g_pipelinesDropped;
extern std::atomic<uint32_t> g_pipelinesCurrentlyCompiling;
extern std::string g_pipelineDebugText;
extern Mutex g_debugMutex;
#endif

extern std::atomic<uint32_t> g_compilingPipelineTaskCount;
extern std::atomic<uint32_t> g_pendingPipelineTaskCount;

// Functions
template<typename T>
static void SetDirtyValue(bool& dirtyState, T& dest, const T& src)
{
    if (dest != src)
    {
        dest = src;
        dirtyState = true;
    }
}

void AddBarrier(GuestBaseTexture* texture, plume::RenderTextureLayout layout);
void FlushBarriers();
void SetFramebuffer(GuestSurface* renderTarget, GuestSurface* depthStencil, bool settingForClear);

template<typename T>
static void ExecuteCopyCommandList(const T& function)
{
    std::lock_guard lock(g_copyMutex);

    g_copyCommandList->begin();
    function();
    g_copyCommandList->end();
    g_copyQueue->executeCommandLists(g_copyCommandList.get(), g_copyCommandFence.get());
    g_copyQueue->waitForCommandFence(g_copyCommandFence.get());
}

template<typename F>
static void EnqueueLambda(F&& f)
{
    using LambdaType = std::decay_t<F>;
    auto* lambda = new LambdaType(std::forward<F>(f));

    RenderCommand cmd;
    cmd.type = RenderCommandType::ExecuteLambda;
    cmd.executeLambda.lambdaPtr = lambda;
    cmd.executeLambda.executor = [](void* ptr) {
        (*static_cast<LambdaType*>(ptr))();
    };
    cmd.executeLambda.deleter = [](void* ptr) {
        delete static_cast<LambdaType*>(ptr);
    };
    g_renderQueue.enqueue(cmd);
}
