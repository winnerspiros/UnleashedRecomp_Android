#pragma once
#include <stdafx.h>
#include <kernel/xbox.h>
#include <cpu/guest_thread.h>

uint32_t KeDelayExecutionThread(uint32_t WaitMode, bool Alertable, be<int64_t>* Timeout);
void KeSetBasePriorityThread(GuestThreadHandle* hThread, int priority);
void KeQueryBasePriorityThread();
uint32_t NtSuspendThread(GuestThreadHandle* hThread, uint32_t* suspendCount);
uint32_t NtResumeThread(GuestThreadHandle* hThread, uint32_t* suspendCount);
uint32_t KeResumeThread(GuestThreadHandle* object);
uint32_t KeSetAffinityThread(uint32_t Thread, uint32_t Affinity, be<uint32_t>* lpPreviousAffinity);
uint32_t KeTlsGetValue(uint32_t dwTlsIndex);
uint32_t KeTlsSetValue(uint32_t dwTlsIndex, uint32_t lpTlsValue);
uint32_t KeTlsAlloc();
uint32_t KeTlsFree(uint32_t dwTlsIndex);
uint32_t KiApcNormalRoutineNop();
void ExTerminateThread();
uint32_t ExCreateThread(be<uint32_t>* handle, uint32_t stackSize, be<uint32_t>* threadId, uint32_t xApiThreadStartup, uint32_t startAddress, uint32_t startContext, uint32_t creationFlags);
