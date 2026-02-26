#pragma once
#include <stdafx.h>
#include <kernel/xbox.h>
#include <kernel/xdm.h>

struct Event final : KernelObject, HostObject<XKEVENT>
{
    bool manualReset;
    std::atomic<bool> signaled;

    Event(XKEVENT* header);
    Event(bool manualReset, bool initialState);

    uint32_t Wait(uint32_t timeout) override;
    bool Set();
    bool Reset();
};

struct Semaphore final : KernelObject, HostObject<XKSEMAPHORE>
{
    std::atomic<uint32_t> count;
    uint32_t maximumCount;

    Semaphore(XKSEMAPHORE* semaphore);
    Semaphore(uint32_t count, uint32_t maximumCount);

    uint32_t Wait(uint32_t timeout) override;
    void Release(uint32_t releaseCount, uint32_t* previousCount);
};

uint32_t NtCreateEvent(be<uint32_t>* handle, void* objAttributes, uint32_t eventType, uint32_t initialState);
uint32_t NtClearEvent(Event* handle, uint32_t* previousState);
uint32_t NtSetEvent(Event* handle, uint32_t* previousState);
uint32_t NtCreateSemaphore(be<uint32_t>* Handle, XOBJECT_ATTRIBUTES* ObjectAttributes, uint32_t InitialCount, uint32_t MaximumCount);
uint32_t NtReleaseSemaphore(Semaphore* Handle, uint32_t ReleaseCount, int32_t* PreviousCount);
uint32_t KeReleaseSemaphore(XKSEMAPHORE* semaphore, uint32_t increment, uint32_t adjustment, uint32_t wait);
void KeInitializeSemaphore(XKSEMAPHORE* semaphore, uint32_t count, uint32_t limit);
void KfReleaseSpinLock(uint32_t* spinLock);
void KfAcquireSpinLock(uint32_t* spinLock);
void KeReleaseSpinLockFromRaisedIrql(uint32_t* spinLock);
void KeAcquireSpinLockAtRaisedIrql(uint32_t* spinLock);
void RtlLeaveCriticalSection(XRTL_CRITICAL_SECTION* cs);
void RtlEnterCriticalSection(XRTL_CRITICAL_SECTION* cs);
bool RtlTryEnterCriticalSection(XRTL_CRITICAL_SECTION* cs);
uint32_t RtlInitializeCriticalSection(XRTL_CRITICAL_SECTION* cs);
void RtlInitializeCriticalSectionAndSpinCount(XRTL_CRITICAL_SECTION* cs, uint32_t spinCount);
void KeLockL2();
void KeUnlockL2();
bool KeSetEvent(XKEVENT* pEvent, uint32_t Increment, bool Wait);
bool KeResetEvent(XKEVENT* pEvent);
uint32_t KeWaitForSingleObject(XDISPATCHER_HEADER* Object, uint32_t WaitReason, uint32_t WaitMode, bool Alertable, be<int64_t>* Timeout);
uint32_t KeWaitForMultipleObjects(uint32_t Count, xpointer<XDISPATCHER_HEADER>* Objects, uint32_t WaitType, uint32_t WaitReason, uint32_t WaitMode, uint32_t Alertable, be<int64_t>* Timeout);
uint32_t KeRaiseIrqlToDpcLevel();
void KfLowerIrql();
void KeLeaveCriticalRegion();
void KeEnterCriticalRegion();
uint32_t NtWaitForSingleObjectEx(uint32_t Handle, uint32_t WaitMode, uint32_t Alertable, be<int64_t>* Timeout);
void NtWaitForMultipleObjectsEx();
