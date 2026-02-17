#include "stdafx.h"
#include <kernel/function.h>
#include <kernel/xdm.h>
#include "misc_funcs.h"

GUEST_FUNCTION_HOOK(sub_831B0ED0, memcpy);
GUEST_FUNCTION_HOOK(sub_831CCB98, memcpy);
GUEST_FUNCTION_HOOK(sub_831CEAE0, memcpy);
GUEST_FUNCTION_HOOK(sub_831CEE04, memcpy);
GUEST_FUNCTION_HOOK(sub_831CF2D0, memcpy);
GUEST_FUNCTION_HOOK(sub_831CF660, memcpy);
GUEST_FUNCTION_HOOK(sub_831B1358, memcpy);
GUEST_FUNCTION_HOOK(sub_831B5E00, memmove);
GUEST_FUNCTION_HOOK(sub_831B0BA0, memset);
GUEST_FUNCTION_HOOK(sub_831CCAA0, memset);

#ifdef _WIN32
GUEST_FUNCTION_HOOK(sub_82BD4CA8, OutputDebugStringA);
#else
GUEST_FUNCTION_STUB(sub_82BD4CA8);
#endif

GUEST_FUNCTION_HOOK(sub_82BD4AC8, QueryPerformanceCounterImpl);
GUEST_FUNCTION_HOOK(sub_831CD040, QueryPerformanceFrequencyImpl);
GUEST_FUNCTION_HOOK(sub_831CDAD0, GetTickCountImpl);

GUEST_FUNCTION_HOOK(sub_82BD4BC0, GlobalMemoryStatusImpl);

// sprintf
PPC_FUNC(sub_82BD4AE8)
{
    sub_831B1630(ctx, base);
}
