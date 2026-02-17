#include <user/config.h>
#include <kernel/function.h>
#include <os/media.h>
#include <os/version.h>
#include <patches/audio_patches.h>
#include <api/SWA.h>

int AudioPatches::m_isAttenuationSupported = -1;

static be<float>* GetVolume(bool isMusic = true)
{
    // Address of the pointer to the audio subsystem class instance.
    constexpr uint32_t ptrToAudioSubsystem = 0x83362FFC;

    auto ppAudioSubsystem = (be<uint32_t>*)g_memory.Translate(ptrToAudioSubsystem);
    uint32_t audioSubsystemAddr = ppAudioSubsystem->get();

    if (!audioSubsystemAddr)
        return nullptr;

    // Derived from sub_82E58728.
    // The audio subsystem has a pointer at offset 0x4 that points to the volume control structure.
    constexpr uint32_t volumeStructPtrOffset = 0x4;
    auto ppVolumeStruct = (be<uint32_t>*)g_memory.Translate(audioSubsystemAddr + volumeStructPtrOffset);
    uint32_t volumeBaseAddr = ppVolumeStruct->get();

    if (!volumeBaseAddr)
        return nullptr;

    // Volume floats are at index 28 (Effects) and 29 (Music) in the volume structure.
    constexpr int IndexEffects = 28; // 0x1C
    constexpr int IndexMusic = 29;   // 0x1D

    int index = isMusic ? IndexMusic : IndexEffects;
    uint32_t volumeAddr = volumeBaseAddr + (index * sizeof(float));

    return (be<float>*)g_memory.Translate(volumeAddr);
}

bool AudioPatches::CanAttenuate()
{
#if _WIN32
    if (m_isAttenuationSupported >= 0)
        return m_isAttenuationSupported;

    auto version = os::version::GetOSVersion();

    m_isAttenuationSupported = version.Major >= 10 && version.Build >= 17763;

    return m_isAttenuationSupported;
#else
    return false;
#endif
}

void AudioPatches::Update(float deltaTime)
{
    auto pMusicVolume = GetVolume();
    auto pEffectsVolume = GetVolume(false);

    if (!pMusicVolume || !pEffectsVolume)
        return;

    if (Config::MusicAttenuation && CanAttenuate())
    {
        auto time = 1.0f - expf(2.5f * -deltaTime);

        if (os::media::IsExternalMediaPlaying())
        {
            *pMusicVolume = std::lerp(*pMusicVolume, 0.0f, time);
        }
        else
        {
            *pMusicVolume = std::lerp(*pMusicVolume, Config::MusicVolume, time);
        }
    }
    else
    {
        *pMusicVolume = Config::MusicVolume;
    }

    *pEffectsVolume = Config::EffectsVolume;
}

// Stub volume setter.
GUEST_FUNCTION_STUB(sub_82E58728);

// HORRIBLE HACK ZONE
// The options menu uses se_system_worldmap.csb, which is stored in Title.ar.00.
// This archive gets unloaded in stages, which causes sounds to not play in the options menu.
// To solve this, once the CSB gets loaded at title, we'll keep it PERMANENTLY loaded.
// This'll make the SFX not work if Title never gets loaded, but that'll only happen when quick booting to stages.
static bool g_loadedWorldMapCsb;

bool MakeCueSheetDataMidAsmHook(PPCRegister& r31)
{
    uint8_t* base = g_memory.base;
    uint32_t str = PPC_LOAD_U32(r31.u32);

    if (str != NULL && strcmp(reinterpret_cast<const char*>(base + str), "se_system_worldmap") == 0)
    {
        if (!g_loadedWorldMapCsb)
        {
            g_loadedWorldMapCsb = true;
            return false; // Allow load for the first and only time.
        }

        // Already loaded before, skip all the loading and name assignment code.
        // Not assigning the name prevents it from unloading the CSB file.
        return true;
    }

    return false;
}
