#include "kernel/video.h"
#include <user/config.h>
#include <os/logger.h>

void VdHSIOCalibrationLock()
{
    LOG_UTILITY("!!! STUB !!!");
}

void XGetVideoMode()
{
    LOG_UTILITY("!!! STUB !!!");
}

uint32_t XGetGameRegion()
{
    if (Config::Language == ELanguage::Japanese)
        return 0x0101;

    return 0x03FF;
}

uint32_t XGetAVPack()
{
    return 0;
}

bool VdPersistDisplay(uint32_t a1, uint32_t* a2)
{
    *a2 = NULL;
    return false;
}

void VdSwap()
{
    LOG_UTILITY("!!! STUB !!!");
}

void VdGetSystemCommandBuffer()
{
    LOG_UTILITY("!!! STUB !!!");
}

void VdEnableRingBufferRPtrWriteBack()
{
    LOG_UTILITY("!!! STUB !!!");
}

void VdInitializeRingBuffer()
{
    LOG_UTILITY("!!! STUB !!!");
}

void VdSetSystemCommandBufferGpuIdentifierAddress()
{
    LOG_UTILITY("!!! STUB !!!");
}

void VdShutdownEngines()
{
    LOG_UTILITY("!!! STUB !!!");
}

void VdQueryVideoMode(XVIDEO_MODE* vm)
{
    memset(vm, 0, sizeof(XVIDEO_MODE));
    vm->DisplayWidth = 1280;
    vm->DisplayHeight = 720;
    vm->IsInterlaced = false;
    vm->IsWidescreen = true;
    vm->IsHighDefinition = true;
    vm->RefreshRate = 0x42700000;
    vm->VideoStandard = 1;
    vm->Unknown4A = 0x4A;
    vm->Unknown01 = 0x01;
}

void VdGetCurrentDisplayInformation()
{
    LOG_UTILITY("!!! STUB !!!");
}

void VdSetDisplayMode()
{
    LOG_UTILITY("!!! STUB !!!");
}

void VdSetGraphicsInterruptCallback()
{
    LOG_UTILITY("!!! STUB !!!");
}

void VdInitializeEngines()
{
    LOG_UTILITY("!!! STUB !!!");
}

void VdIsHSIOTrainingSucceeded()
{
    LOG_UTILITY("!!! STUB !!!");
}

void VdGetCurrentDisplayGamma()
{
    LOG_UTILITY("!!! STUB !!!");
}

void VdQueryVideoFlags()
{
    LOG_UTILITY("!!! STUB !!!");
}

void VdCallGraphicsNotificationRoutines()
{
    LOG_UTILITY("!!! STUB !!!");
}

void VdInitializeScalerCommandBuffer()
{
    LOG_UTILITY("!!! STUB !!!");
}

uint32_t VdRetrainEDRAM()
{
    return 0;
}

void VdRetrainEDRAMWorker()
{
    LOG_UTILITY("!!! STUB !!!");
}

void VdEnableDisableClockGating()
{
    LOG_UTILITY("!!! STUB !!!");
}
