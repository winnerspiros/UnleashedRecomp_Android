#pragma once
#include <stdafx.h>
#include <kernel/xbox.h>

void VdHSIOCalibrationLock();
void XGetVideoMode();
uint32_t XGetGameRegion();
uint32_t XGetAVPack();
bool VdPersistDisplay(uint32_t a1, uint32_t* a2);
void VdSwap();
void VdGetSystemCommandBuffer();
void VdEnableRingBufferRPtrWriteBack();
void VdInitializeRingBuffer();
void VdSetSystemCommandBufferGpuIdentifierAddress();
void VdShutdownEngines();
void VdQueryVideoMode(XVIDEO_MODE* vm);
void VdGetCurrentDisplayInformation();
void VdSetDisplayMode();
void VdSetGraphicsInterruptCallback();
void VdInitializeEngines();
void VdIsHSIOTrainingSucceeded();
void VdGetCurrentDisplayGamma();
void VdQueryVideoFlags();
void VdCallGraphicsNotificationRoutines();
void VdInitializeScalerCommandBuffer();
uint32_t VdRetrainEDRAM();
void VdRetrainEDRAMWorker();
void VdEnableDisableClockGating();
