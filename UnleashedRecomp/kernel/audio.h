#pragma once
#include <stdafx.h>
#include <kernel/xbox.h>

void XAudioGetVoiceCategoryVolume();
uint32_t XAudioGetVoiceCategoryVolumeChangeMask(uint32_t Driver, be<uint32_t>* Mask);

// Externs for hooks
extern uint32_t XMAReleaseContext(uint32_t pContext);
extern uint32_t XMACreateContext(uint32_t dwSize, uint32_t pContextData, uint32_t pContext);
extern uint32_t XAudioRegisterRenderDriverClient(uint32_t pClient);
extern uint32_t XAudioUnregisterRenderDriverClient(uint32_t pClient);
extern uint32_t XAudioSubmitRenderDriverFrame(uint32_t pFrame);
