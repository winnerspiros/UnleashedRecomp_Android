#pragma once
#include "win_types.h"
#include <xbox.h>

uint32_t QueryPerformanceCounterImpl(LARGE_INTEGER* lpPerformanceCount);
uint32_t QueryPerformanceFrequencyImpl(LARGE_INTEGER* lpFrequency);
uint32_t GetTickCountImpl();
void GlobalMemoryStatusImpl(XLPMEMORYSTATUS lpMemoryStatus);
