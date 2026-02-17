#include "paths.h"
#include <os/process.h>

#ifdef ANDROID
#include <SDL.h>
#endif

std::filesystem::path g_executableRoot = os::process::GetExecutableRoot();
std::filesystem::path g_userPath = BuildUserPath();

bool CheckPortable()
{
    return std::filesystem::exists(g_executableRoot / "portable.txt");
}

std::filesystem::path BuildUserPath()
{
    if (CheckPortable())
        return g_executableRoot;

    std::filesystem::path userPath;

#if defined(ANDROID)
    char* prefPath = SDL_GetPrefPath("hedge-dev", "UnleashedRecomp");
    if (prefPath)
    {
        userPath = std::filesystem::path(prefPath);
        SDL_free(prefPath);
    }
#else
    static_assert(false, "GetUserPath() not implemented for this platform.");
#endif

    return userPath;
}

const std::filesystem::path& GetUserPath()
{
    return g_userPath;
}
