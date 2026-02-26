#pragma once
#include <filesystem>

inline std::filesystem::path GetSavePath(bool) { return "."; }
inline std::filesystem::path GetGamePath() { return "."; }
inline std::filesystem::path GetUserPath() { return "."; } // Added for completeness if needed
