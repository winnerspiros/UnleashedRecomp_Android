#include <cassert>

// CONFIG_DEFINE
template<typename T, bool isHidden>
ConfigDef<T, isHidden>::ConfigDef(std::string section, std::string name, T defaultValue) : Section(std::move(section)), Name(std::move(name)), DefaultValue(defaultValue)
{
    g_configDefinitions.emplace_back(this);
}

// CONFIG_DEFINE_LOCALISED
template<typename T, bool isHidden>
ConfigDef<T, isHidden>::ConfigDef(std::string section, std::string name, CONFIG_LOCALE* nameLocale, T defaultValue) : Section(std::move(section)), Name(std::move(name)), Locale(nameLocale), DefaultValue(defaultValue)
{
    g_configDefinitions.emplace_back(this);
}

// CONFIG_DEFINE_ENUM
template<typename T, bool isHidden>
ConfigDef<T, isHidden>::ConfigDef(std::string section, std::string name, T defaultValue, std::unordered_map<std::string, T>* enumTemplate) : Section(std::move(section)), Name(std::move(name)), DefaultValue(defaultValue), EnumTemplate(enumTemplate)
{
    for (const auto& pair : *EnumTemplate)
        EnumTemplateReverse[pair.second] = pair.first;

    g_configDefinitions.emplace_back(this);
}

// CONFIG_DEFINE_ENUM_LOCALISED
template<typename T, bool isHidden>
ConfigDef<T, isHidden>::ConfigDef(std::string section, std::string name, CONFIG_LOCALE* nameLocale, T defaultValue, std::unordered_map<std::string, T>* enumTemplate, CONFIG_ENUM_LOCALE(T)* enumLocale) : Section(std::move(section)), Name(std::move(name)), Locale(nameLocale), DefaultValue(defaultValue), EnumTemplate(enumTemplate), EnumLocale(enumLocale)
{
    for (const auto& pair : *EnumTemplate)
        EnumTemplateReverse[pair.second] = pair.first;

    g_configDefinitions.emplace_back(this);
}

template<typename T, bool isHidden>
ConfigDef<T, isHidden>::~ConfigDef() = default;

template<typename T, bool isHidden>
bool ConfigDef<T, isHidden>::IsHidden()
{
    return isHidden && !IsLoadedFromConfig;
}

template<typename T, bool isHidden>
void ConfigDef<T, isHidden>::ReadValue(toml::v3::ex::parse_result& toml)
{
    if (auto pSection = toml[Section].as_table())
    {
        const auto& section = *pSection;

        if constexpr (std::is_same<T, std::string>::value)
        {
            Value = section[Name].value_or(DefaultValue);
        }
        else if constexpr (std::is_enum_v<T>)
        {
            auto value = section[Name].value_or(std::string());
            auto it = EnumTemplate->find(value);

            if (it != EnumTemplate->end())
            {
                Value = it->second;
            }
            else
            {
                Value = DefaultValue;
            }
        }
        else
        {
            Value = section[Name].value_or(DefaultValue);
        }

        if (Callback)
            Callback(this);

        if (pSection->contains(Name))
            IsLoadedFromConfig = true;
    }
}

template<typename T, bool isHidden>
void ConfigDef<T, isHidden>::MakeDefault()
{
    Value = DefaultValue;

    if constexpr (std::is_enum_v<T>)
        SnapToNearestAccessibleValue(false);
}

template<typename T, bool isHidden>
std::string_view ConfigDef<T, isHidden>::GetSection() const
{
    return Section;
}

template<typename T, bool isHidden>
std::string_view ConfigDef<T, isHidden>::GetName() const
{
    return Name;
}

template<typename T, bool isHidden>
std::string ConfigDef<T, isHidden>::GetNameLocalised(ELanguage language) const
{
    if (Locale != nullptr)
    {
        auto languageFindResult = Locale->find(language);
        if (languageFindResult == Locale->end())
            languageFindResult = Locale->find(ELanguage::English);

        if (languageFindResult != Locale->end())
            return std::get<0>(languageFindResult->second);
    }

    return Name;
}

template<typename T, bool isHidden>
std::string ConfigDef<T, isHidden>::GetDescription(ELanguage language) const
{
    if (Locale != nullptr)
    {
        auto languageFindResult = Locale->find(language);
        if (languageFindResult == Locale->end())
            languageFindResult = Locale->find(ELanguage::English);

        if (languageFindResult != Locale->end())
            return std::get<1>(languageFindResult->second);
    }

    return "";
}

template<typename T, bool isHidden>
bool ConfigDef<T, isHidden>::IsDefaultValue() const
{
    return Value == DefaultValue;
}

template<typename T, bool isHidden>
const void* ConfigDef<T, isHidden>::GetValue() const
{
    return &Value;
}

template<typename T, bool isHidden>
std::string ConfigDef<T, isHidden>::GetValueLocalised(ELanguage language) const
{
    CONFIG_ENUM_LOCALE(T)* locale = nullptr;

    if constexpr (std::is_enum_v<T>)
    {
        locale = EnumLocale;
    }
    else if constexpr (std::is_same_v<T, bool>)
    {
        return Value
            ? Localise("Common_On")
            : Localise("Common_Off");
    }

    if (locale != nullptr)
    {
        ELanguage languages[] = { language, ELanguage::English };

        for (auto languageToFind : languages)
        {
            auto languageFindResult = locale->find(languageToFind);

            if (languageFindResult != locale->end())
            {
                auto valueFindResult = languageFindResult->second.find(Value);
                if (valueFindResult != languageFindResult->second.end())
                    return std::get<0>(valueFindResult->second);
            }

            if (languageToFind == ELanguage::English)
                break;
        }
    }

    return ToString(false);
}

template<typename T, bool isHidden>
std::string ConfigDef<T, isHidden>::GetValueDescription(ELanguage language) const
{
    CONFIG_ENUM_LOCALE(T)* locale = nullptr;

    if constexpr (std::is_enum_v<T>)
    {
        locale = EnumLocale;
    }
    else if constexpr (std::is_same_v<T, bool>)
    {
        return "";
    }

    if (locale != nullptr)
    {
        ELanguage languages[] = { language, ELanguage::English };

        for (auto languageToFind : languages)
        {
            auto languageFindResult = locale->find(languageToFind);

            if (languageFindResult != locale->end())
            {
                auto valueFindResult = languageFindResult->second.find(Value);
                if (valueFindResult != languageFindResult->second.end())
                    return std::get<1>(valueFindResult->second);
            }

            if (languageToFind == ELanguage::English)
                break;
        }
    }

    return "";
}

template<typename T, bool isHidden>
std::string ConfigDef<T, isHidden>::GetDefinition(bool withSection) const
{
    std::string result;

    if (withSection)
        result += "[" + Section + "]\n";

    result += Name + " = " + ToString();

    return result;
}

template<typename T, bool isHidden>
std::string ConfigDef<T, isHidden>::ToString(bool strWithQuotes) const
{
    std::string result = "N/A";

    if constexpr (std::is_same_v<T, std::string>)
    {
        result = fmt::format("{}", Value);

        if (strWithQuotes)
            result = fmt::format("\"{}\"", result);
    }
    else if constexpr (std::is_enum_v<T>)
    {
        auto it = EnumTemplateReverse.find(Value);

        if (it != EnumTemplateReverse.end())
            result = fmt::format("{}", it->second);

        if (strWithQuotes)
            result = fmt::format("\"{}\"", result);
    }
    else
    {
        result = fmt::format("{}", Value);
    }

    return result;
}

template<typename T, bool isHidden>
void ConfigDef<T, isHidden>::GetLocaleStrings(std::vector<std::string_view>& localeStrings) const
{
    if (Locale != nullptr)
    {
        for (auto& [language, nameAndDesc] : *Locale)
        {
            localeStrings.push_back(std::get<0>(nameAndDesc));
            localeStrings.push_back(std::get<1>(nameAndDesc));
        }
    }

    if (EnumLocale != nullptr)
    {
        for (auto& [language, locale] : *EnumLocale)
        {
            for (auto& [value, nameAndDesc] : locale)
            {
                localeStrings.push_back(std::get<0>(nameAndDesc));
                localeStrings.push_back(std::get<1>(nameAndDesc));
            }
        }
    }
}

template<typename T, bool isHidden>
void ConfigDef<T, isHidden>::SnapToNearestAccessibleValue(bool searchUp)
{
    if constexpr (std::is_enum_v<T>)
    {
        if (EnumTemplateReverse.empty() || InaccessibleValues.empty())
            return;

        if (EnumTemplateReverse.size() == InaccessibleValues.size())
        {
            assert(false && "All enum values are marked inaccessible and the nearest accessible value cannot be determined.");
            return;
        }

        auto it = EnumTemplateReverse.find(Value);

        if (it == EnumTemplateReverse.end())
        {
            assert(false && "Enum value does not exist in the template.");
            return;
        }

        // Skip the enum value if it's marked as inaccessible.
        while (InaccessibleValues.find(it->first) != InaccessibleValues.end())
        {
            if (searchUp)
            {
                ++it;

                if (it == EnumTemplateReverse.end())
                    it = EnumTemplateReverse.begin();
            }
            else
            {
                if (it == EnumTemplateReverse.begin())
                    it = EnumTemplateReverse.end();

                --it;
            }
        }

        Value = it->first;
    }
}
