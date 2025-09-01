#include "I18nManager.h"
#include <filesystem>
#include <ll/api/Config.h>
#include <ll/api/Logger.h>
#include <ll/api/mod/NativeMod.h>


namespace carpet_mod_for_ll {

std::unique_ptr<I18nManager> I18nManager::instance = nullptr;

I18nManager::I18nManager() {
    // 获取语言文件目录
    auto& mod     = ll::mod::NativeMod::current();
    auto  langDir = mod.getModDir() / "assets" / "lang";

    if (!std::filesystem::exists(langDir)) {
        mod.getLogger().warn("Language directory not found: {}", langDir.string());
        return;
    }

    // 初始化 i18n
    try {
        i18n.loadFromDirectory(langDir);
        i18n.setDefaultLocaleName("en_US");

        // 尝试设置系统语言
        auto sysLang = ll::i18n::getSystemLocaleName();
        if (i18n.hasLocale(sysLang)) {
            i18n.setLocaleName(sysLang);
        }

        mod.getLogger().info("I18n initialized with {} languages", i18n.getLocaleNames().size());
    } catch (const std::exception& e) {
        mod.getLogger().error("Failed to initialize I18n: {}", e.what());
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

std::string I18nManager::tr(const std::string& key) const { return i18n.get(key); }

std::string I18nManager::tr(const std::string& key, const std::vector<std::string>& args) const {
    return i18n.get(key, args);
}

void I18nManager::setLanguage(const std::string& lang) {
    if (i18n.hasLocale(lang)) {
        i18n.setLocaleName(lang);
    } else {
        auto& mod = ll::mod::NativeMod::current();
        mod.getLogger().warn("Language '{}' not available, using default", lang);
    }
}

std::string I18nManager::getCurrentLanguage() const { return i18n.getLocaleName(); }

std::vector<std::string> I18nManager::getAvailableLanguages() const { return i18n.getLocaleNames(); }

bool I18nManager::reload() {
    try {
        auto& mod     = ll::mod::NativeMod::current();
        auto  langDir = mod.getModDir() / "assets" / "lang";

        i18n.loadFromDirectory(langDir);
        mod.getLogger().info("I18n reloaded successfully");
        return true;
    } catch (const std::exception& e) {
        auto& mod = ll::mod::NativeMod::current();
        mod.getLogger().error("Failed to reload I18n: {}", e.what());
        return false;
    }
}

} // namespace carpet_mod_for_ll