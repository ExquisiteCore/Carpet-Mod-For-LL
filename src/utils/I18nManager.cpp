#include "I18nManager.h"
#include <fstream>
#include <ll/api/i18n/I18n.h>
#include <ll/api/mod/NativeMod.h>
#include <nlohmann/json.hpp>

namespace carpet_mod_for_ll {

std::unique_ptr<I18nManager> I18nManager::instance = nullptr;

I18nManager::I18nManager() : currentLocale("zh_CN") {
    auto mod = ll::mod::NativeMod::current();

    try {
        // 设置默认语言为系统语言或中文
        auto systemLang = std::string(ll::i18n::getDefaultLocaleCode());
        if (!systemLang.empty()) {
            currentLocale = systemLang;
        }

        // 重新加载所有语言文件
        reload();

        mod->getLogger().info("I18n system initialized with locale: {}", currentLocale);
    } catch (const std::exception& e) {
        mod->getLogger().error("Failed to initialize I18n: {}", e.what());
    }
}

I18nManager& I18nManager::getInstance() {
    if (!instance) {
        instance = std::make_unique<I18nManager>();
    }
    return *instance;
}

void I18nManager::initialize() {
    getInstance(); // 触发初始化
}

void I18nManager::cleanup() { instance.reset(); }

std::string I18nManager::tr(const std::string& key) const {
    auto localeIt = translations.find(currentLocale);
    if (localeIt != translations.end()) {
        auto keyIt = localeIt->second.find(key);
        if (keyIt != localeIt->second.end()) {
            return keyIt->second;
        }
    }

    // 回退到默认语言（中文，然后英文）
    if (currentLocale != "zh_CN") {
        auto defaultIt = translations.find("zh_CN");
        if (defaultIt != translations.end()) {
            auto keyIt = defaultIt->second.find(key);
            if (keyIt != defaultIt->second.end()) {
                return keyIt->second;
            }
        }
    }
    
    // 再回退到英文
    if (currentLocale != "en_US") {
        auto defaultIt = translations.find("en_US");
        if (defaultIt != translations.end()) {
            auto keyIt = defaultIt->second.find(key);
            if (keyIt != defaultIt->second.end()) {
                return keyIt->second;
            }
        }
    }

    // 都找不到，返回键本身
    return key;
}

std::string I18nManager::tr(const std::string& key, const std::vector<std::string>& args) const {
    std::string text = tr(key);
    return formatText(text, args);
}

void I18nManager::setLanguage(const std::string& lang) {
    if (translations.find(lang) != translations.end()) {
        currentLocale = lang;
        auto mod      = ll::mod::NativeMod::current();
        mod->getLogger().info("Language switched to: {}", lang);
    } else {
        auto mod = ll::mod::NativeMod::current();
        mod->getLogger().warn("Language '{}' not available", lang);
    }
}

std::string I18nManager::getCurrentLanguage() const { return currentLocale; }

std::vector<std::string> I18nManager::getAvailableLanguages() const {
    std::vector<std::string> languages;
    languages.reserve(translations.size());

    for (const auto& pair : translations) {
        languages.push_back(pair.first);
    }

    return languages;
}

bool I18nManager::reload() {
    try {
        auto mod     = ll::mod::NativeMod::current();
        auto langDir = mod->getModDir() / "assets" / "lang";

        // 清空现有翻译
        translations.clear();

        // 扫描语言文件目录
        if (std::filesystem::exists(langDir) && std::filesystem::is_directory(langDir)) {
            for (const auto& entry : std::filesystem::directory_iterator(langDir)) {
                if (entry.is_regular_file() && entry.path().extension() == ".json") {
                    auto locale = entry.path().stem().string();
                    loadLanguageFile(locale, entry.path());
                }
            }
        }

        // 确保至少有中文和英文语言文件
        if (translations.find("zh_CN") == translations.end()) {
            mod->getLogger().warn("No zh_CN language file found, creating minimal fallback");
            translations["zh_CN"] = {
                {"carpet.error.permission",   "您没有权限使用此命令"},
                {"carpet.error.invalid_args", "无效参数: {}"},
                {"carpet.error.player_only",  "此命令只能由玩家使用"},
                {"carpet.error.console_only", "此命令只能从控制台使用"}
            };
        }
        
        if (translations.find("en_US") == translations.end()) {
            mod->getLogger().warn("No en_US language file found, creating minimal fallback");
            translations["en_US"] = {
                {"carpet.error.permission",   "You don't have permission to use this command"},
                {"carpet.error.invalid_args", "Invalid argument: {}"},
                {"carpet.error.player_only",  "This command can only be used by players"},
                {"carpet.error.console_only", "This command can only be used from console"}
            };
        }

        mod->getLogger().info("I18n reloaded successfully with {} languages", translations.size());
        return true;
    } catch (const std::exception& e) {
        auto mod = ll::mod::NativeMod::current();
        mod->getLogger().error("Failed to reload I18n: {}", e.what());
        return false;
    }
}

bool I18nManager::loadLanguageFile(const std::string& locale, const std::filesystem::path& filePath) {
    try {
        std::ifstream file(filePath);
        if (!file.is_open()) {
            return false;
        }

        nlohmann::json json;
        file >> json;

        auto& localeMap = translations[locale];

        // 递归处理JSON对象，将嵌套的键转换为点分隔的键
        std::function<void(const nlohmann::json&, const std::string&)> processJson = [&](const nlohmann::json& obj,
                                                                                         const std::string&    prefix) {
            for (const auto& [key, value] : obj.items()) {
                std::string fullKey = prefix.empty() ? key : prefix + "." + key;

                if (value.is_string()) {
                    localeMap[fullKey] = value.get<std::string>();
                } else if (value.is_object()) {
                    processJson(value, fullKey);
                }
            }
        };

        processJson(json, "");

        auto mod = ll::mod::NativeMod::current();
        mod->getLogger().info("Loaded {} translations for locale: {}", localeMap.size(), locale);
        return true;

    } catch (const std::exception& e) {
        auto mod = ll::mod::NativeMod::current();
        mod->getLogger().error("Failed to load language file {}: {}", filePath.string(), e.what());
        return false;
    }
}

std::string I18nManager::formatText(const std::string& text, const std::vector<std::string>& args) const {
    if (args.empty()) {
        return text;
    }

    std::string result = text;

    // 替换 {} 占位符
    size_t argIndex = 0;
    size_t pos      = 0;
    while ((pos = result.find("{}", pos)) != std::string::npos && argIndex < args.size()) {
        result.replace(pos, 2, args[argIndex]);
        pos += args[argIndex].length();
        argIndex++;
    }

    return result;
}

} // namespace carpet_mod_for_ll