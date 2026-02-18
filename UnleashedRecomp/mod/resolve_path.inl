enum class ModType
{
    HMM,
    UMM
};

struct Mod
{
    ModType type{};
    std::vector<std::filesystem::path> includeDirs;
    bool merge = false;
    ankerl::unordered_dense::set<std::filesystem::path> readOnly;
};

static std::vector<Mod> g_mods;
static ankerl::unordered_dense::map<std::string, std::vector<std::pair<size_t, std::filesystem::path>>> g_modFileIndex;

std::filesystem::path ModLoader::ResolvePath(std::string_view path)
{
    std::string_view root;

    size_t sepIndex = path.find(":\\");
    if (sepIndex != std::string_view::npos)
    {
        root = path.substr(0, sepIndex);
        path.remove_prefix(sepIndex + 2);
    }

    if (root == "save")
    {
        if (!ModLoader::s_saveFilePath.empty())
        {
            if (path == "SYS-DATA")
                return ModLoader::s_saveFilePath;
            else
                return ModLoader::s_saveFilePath.parent_path() / path;
        }

        return {};
    }

    if (g_mods.empty())
        return {};

    thread_local xxHashMap<std::filesystem::path> s_cache;

    XXH64_hash_t hash = XXH3_64bits(path.data(), path.size());
    auto findResult = s_cache.find(hash);
    if (findResult != s_cache.end())
        return findResult->second;

    std::string pathStr(path);
    std::replace(pathStr.begin(), pathStr.end(), '\\', '/');

    std::string key = pathStr;
    std::transform(key.begin(), key.end(), key.begin(), [](unsigned char c) { return std::tolower(c); });

    auto it = g_modFileIndex.find(key);
    if (it != g_modFileIndex.end())
    {
        std::filesystem::path fsPath(pathStr);

        bool canBeMerged =
            path.find(".arl") == (path.size() - 4) ||
            path.find(".ar.") == (path.size() - 6) ||
            path.find(".ar") == (path.size() - 3);

        for (const auto& [modIndex, fullPath] : it->second)
        {
            const auto& mod = g_mods[modIndex];
            if (mod.type == ModType::UMM && mod.merge && canBeMerged && !mod.readOnly.contains(fsPath))
                continue;

            return s_cache.emplace(hash, fullPath).first->second;
        }
    }

    return s_cache.emplace(hash, std::filesystem::path{}).first->second;
}

std::vector<std::filesystem::path>* ModLoader::GetIncludeDirectories(size_t modIndex)
{
    return modIndex < g_mods.size() ? &g_mods[modIndex].includeDirs : nullptr;
}
