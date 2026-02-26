#include "kernel/synchronization.h"
#include <kernel/xdm.h>
#include <cpu/guest_thread.h>
#include <cpu/ppc_context.h>
#include <os/logger.h>
#include <kernel/utilities.h>

#ifdef _WIN32
#include <ntstatus.h>
#endif

static std::atomic<uint32_t> g_dispatcherObjectStateGeneration;

Event::Event(XKEVENT* header)
    : manualReset(!header->Type), signaled(!!header->SignalState)
{
}

Event::Event(bool manualReset, bool initialState)
    : manualReset(manualReset), signaled(initialState)
{
}

uint32_t Event::Wait(uint32_t timeout)
{
    if (timeout == 0)
    {
        if (manualReset)
        {
            if (!signaled)
                return STATUS_TIMEOUT;
        }
        else
        {
            bool expected = true;
            if (!signaled.compare_exchange_strong(expected, false))
                return STATUS_TIMEOUT;
        }
    }
    else if (timeout == INFINITE)
    {
        if (manualReset)
        {
            signaled.wait(false);
        }
        else
        {
            while (true)
            {
                bool expected = true;
                if (signaled.compare_exchange_weak(expected, false))
                    break;

                signaled.wait(expected);
            }
        }
    }
    else
    {
        assert(false && "Unhandled timeout value.");
    }

    return STATUS_SUCCESS;
}

bool Event::Set()
{
    signaled = true;

    if (manualReset)
        signaled.notify_all();
    else
        signaled.notify_one();

    return TRUE;
}

bool Event::Reset()
{
    signaled = false;
    return TRUE;
}

Semaphore::Semaphore(XKSEMAPHORE* semaphore)
    : count(semaphore->Header.SignalState), maximumCount(semaphore->Limit)
{
}

Semaphore::Semaphore(uint32_t count, uint32_t maximumCount)
    : count(count), maximumCount(maximumCount)
{
}

uint32_t Semaphore::Wait(uint32_t timeout)
{
    if (timeout == 0)
    {
        uint32_t currentCount = count.load();
        if (currentCount != 0)
        {
            if (count.compare_exchange_weak(currentCount, currentCount - 1))
                return STATUS_SUCCESS;
        }

        return STATUS_TIMEOUT;
    }
    else if (timeout == INFINITE)
    {
        uint32_t currentCount;
        while (true)
        {
            currentCount = count.load();
            if (currentCount != 0)
            {
                if (count.compare_exchange_weak(currentCount, currentCount - 1))
                    return STATUS_SUCCESS;
            }
            else
            {
                count.wait(0);
            }
        }

        return STATUS_SUCCESS;
    }
    else
    {
        assert(false && "Unhandled timeout value.");
        return STATUS_TIMEOUT;
    }
}

void Semaphore::Release(uint32_t releaseCount, uint32_t* previousCount)
{
    if (previousCount != nullptr)
        *previousCount = count;

    assert(count + releaseCount <= maximumCount);

    count += releaseCount;
    count.notify_all();

    ++g_dispatcherObjectStateGeneration;
    g_dispatcherObjectStateGeneration.notify_all();
}

static KernelObject* GetKernelObjectFromHeader(XDISPATCHER_HEADER* header)
{
    switch (header->Type)
    {
    case 0:
    case 1:
        return QueryKernelObject<Event>(*header);
    case 5:
        return QueryKernelObject<Semaphore>(*header);
    default:
        assert(false && "Unrecognized kernel object type.");
        return nullptr;
    }
}

uint32_t NtCreateEvent(be<uint32_t>* handle, void* objAttributes, uint32_t eventType, uint32_t initialState)
{
    *handle = GetKernelHandle(CreateKernelObject<Event>(!eventType, !!initialState));
    return 0;
}

uint32_t NtClearEvent(Event* handle, uint32_t* previousState)
{
    handle->Reset();
    return 0;
}

uint32_t NtSetEvent(Event* handle, uint32_t* previousState)
{
    handle->Set();
    return 0;
}

uint32_t NtCreateSemaphore(be<uint32_t>* Handle, XOBJECT_ATTRIBUTES* ObjectAttributes, uint32_t InitialCount, uint32_t MaximumCount)
{
    *Handle = GetKernelHandle(CreateKernelObject<Semaphore>(InitialCount, MaximumCount));
    return STATUS_SUCCESS;
}

uint32_t NtReleaseSemaphore(Semaphore* Handle, uint32_t ReleaseCount, int32_t* PreviousCount)
{
    uint32_t previousCount;
    Handle->Release(ReleaseCount, &previousCount);

    if (PreviousCount != nullptr)
        *PreviousCount = ByteSwap(previousCount);

    return STATUS_SUCCESS;
}

uint32_t KeReleaseSemaphore(XKSEMAPHORE* semaphore, uint32_t increment, uint32_t adjustment, uint32_t wait)
{
    auto* object = QueryKernelObject<Semaphore>(semaphore->Header);
    object->Release(adjustment, nullptr);
    return STATUS_SUCCESS;
}

void KeInitializeSemaphore(XKSEMAPHORE* semaphore, uint32_t count, uint32_t limit)
{
    semaphore->Header.Type = 5;
    semaphore->Header.SignalState = count;
    semaphore->Limit = limit;

    auto* object = QueryKernelObject<Semaphore>(semaphore->Header);
}

void KfReleaseSpinLock(uint32_t* spinLock)
{
    std::atomic_ref spinLockRef(*spinLock);
    spinLockRef = 0;
}

void KfAcquireSpinLock(uint32_t* spinLock)
{
    std::atomic_ref spinLockRef(*spinLock);

    while (true)
    {
        uint32_t expected = 0;
        if (spinLockRef.compare_exchange_weak(expected, g_ppcContext->r13.u32))
            break;

        std::this_thread::yield();
    }
}

void KeReleaseSpinLockFromRaisedIrql(uint32_t* spinLock)
{
    std::atomic_ref spinLockRef(*spinLock);
    spinLockRef = 0;
}

void KeAcquireSpinLockAtRaisedIrql(uint32_t* spinLock)
{
    std::atomic_ref spinLockRef(*spinLock);

    while (true)
    {
        uint32_t expected = 0;
        if (spinLockRef.compare_exchange_weak(expected, g_ppcContext->r13.u32))
            break;

        std::this_thread::yield();
    }
}

void RtlLeaveCriticalSection(XRTL_CRITICAL_SECTION* cs)
{
    cs->RecursionCount--;

    if (cs->RecursionCount != 0)
        return;

    std::atomic_ref owningThread(cs->OwningThread);
    owningThread.store(0);
    owningThread.notify_one();
}

void RtlEnterCriticalSection(XRTL_CRITICAL_SECTION* cs)
{
    uint32_t thisThread = g_ppcContext->r13.u32;
    assert(thisThread != NULL);

    std::atomic_ref owningThread(cs->OwningThread);

    while (true)
    {
        uint32_t previousOwner = 0;

        if (owningThread.compare_exchange_weak(previousOwner, thisThread) || previousOwner == thisThread)
        {
            cs->RecursionCount++;
            return;
        }

        owningThread.wait(previousOwner);
    }
}

bool RtlTryEnterCriticalSection(XRTL_CRITICAL_SECTION* cs)
{
    uint32_t thisThread = g_ppcContext->r13.u32;
    assert(thisThread != NULL);

    std::atomic_ref owningThread(cs->OwningThread);

    uint32_t previousOwner = 0;

    if (owningThread.compare_exchange_weak(previousOwner, thisThread) || previousOwner == thisThread)
    {
        cs->RecursionCount++;
        return true;
    }

    return false;
}

uint32_t RtlInitializeCriticalSection(XRTL_CRITICAL_SECTION* cs)
{
    cs->Header.Absolute = 0;
    cs->LockCount = -1;
    cs->RecursionCount = 0;
    cs->OwningThread = 0;

    return 0;
}

void RtlInitializeCriticalSectionAndSpinCount(XRTL_CRITICAL_SECTION* cs, uint32_t spinCount)
{
    cs->Header.Absolute = (spinCount + 255) >> 8;
    cs->LockCount = -1;
    cs->RecursionCount = 0;
    cs->OwningThread = 0;
}

void KeLockL2()
{
    LOG_UTILITY("!!! STUB !!!");
}

void KeUnlockL2()
{
    LOG_UTILITY("!!! STUB !!!");
}

bool KeSetEvent(XKEVENT* pEvent, uint32_t Increment, bool Wait)
{
    bool result = QueryKernelObject<Event>(*pEvent)->Set();

    ++g_dispatcherObjectStateGeneration;
    g_dispatcherObjectStateGeneration.notify_all();

    return result;
}

bool KeResetEvent(XKEVENT* pEvent)
{
    return QueryKernelObject<Event>(*pEvent)->Reset();
}

uint32_t KeWaitForSingleObject(XDISPATCHER_HEADER* Object, uint32_t WaitReason, uint32_t WaitMode, bool Alertable, be<int64_t>* Timeout)
{
    const uint32_t timeout = GuestTimeoutToMilliseconds(Timeout);
    assert(timeout == INFINITE);

    switch (Object->Type)
    {
        case 0:
        case 1:
            QueryKernelObject<Event>(*Object)->Wait(timeout);
            break;

        case 5:
            QueryKernelObject<Semaphore>(*Object)->Wait(timeout);
            break;

        default:
            assert(false && "Unrecognized kernel object type.");
            return STATUS_TIMEOUT;
    }

    return STATUS_SUCCESS;
}

uint32_t KeWaitForMultipleObjects(uint32_t Count, xpointer<XDISPATCHER_HEADER>* Objects, uint32_t WaitType, uint32_t WaitReason, uint32_t WaitMode, uint32_t Alertable, be<int64_t>* Timeout)
{
    const uint64_t timeout = GuestTimeoutToMilliseconds(Timeout);
    assert(timeout == INFINITE);

    if (WaitType == 0) // Wait all
    {
        for (size_t i = 0; i < Count; i++)
        {
            auto* object = GetKernelObjectFromHeader(Objects[i]);
            if (object)
                object->Wait(timeout);
        }
    }
    else
    {
        thread_local std::vector<KernelObject*> s_objects;
        s_objects.resize(Count);

        for (size_t i = 0; i < Count; i++)
        {
            s_objects[i] = GetKernelObjectFromHeader(Objects[i]);
        }

        while (true)
        {
            uint32_t generation = g_dispatcherObjectStateGeneration.load();

            for (size_t i = 0; i < Count; i++)
            {
                if (s_objects[i] && s_objects[i]->Wait(0) == STATUS_SUCCESS)
                {
                    return STATUS_WAIT_0 + i;
                }
            }

            g_dispatcherObjectStateGeneration.wait(generation);
        }
    }

    return STATUS_SUCCESS;
}

uint32_t KeRaiseIrqlToDpcLevel()
{
    return 0;
}

void KfLowerIrql() { }

void KeLeaveCriticalRegion()
{
    LOG_UTILITY("!!! STUB !!!");
}

void KeEnterCriticalRegion()
{
    LOG_UTILITY("!!! STUB !!!");
}

uint32_t NtWaitForSingleObjectEx(uint32_t Handle, uint32_t WaitMode, uint32_t Alertable, be<int64_t>* Timeout)
{
    uint32_t timeout = GuestTimeoutToMilliseconds(Timeout);
    assert(timeout == 0 || timeout == INFINITE);

    if (IsKernelObject(Handle))
    {
        return GetKernelObject(Handle)->Wait(timeout);
    }
    else
    {
        assert(false && "Unrecognized handle value.");
    }

    return STATUS_TIMEOUT;
}

void NtWaitForMultipleObjectsEx()
{
    LOG_UTILITY("!!! STUB !!!");
}
