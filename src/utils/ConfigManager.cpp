#include "ConfigManager.h"
#include <LL/api/io/Logger.h>
#include <ll/api/Config.h>
#include <ll/api/mod/NativeMod.h>


namespace carpet_mod_for_ll {

std::unique_ptr<ConfigManager> ConfigManager::instance = nullptr;

ConfigManager::ConfigManager() {
    auto& mod  = ll::mod::NativeMod::current();
    configPath = mod.getConfigDir() / "config.json";
}

ConfigManager& ConfigManager::getInstance() {
    if (!instance) {
        instance = std::make_unique<ConfigManager>();
    }
    return *instance;
}

void ConfigManager::initialize() {
    auto& manager = getInstance();
    manager.load();
}

void ConfigManager::cleanup() {
    if (instance) {
        instance->save();
        instance.reset();
    }
}

bool ConfigManager::load() {
    auto& mod    = ll::mod::NativeMod::current();
    auto& logger = mod.getLogger();

    try {
        if (!std::filesystem::exists(configPath)) {
            logger.info("Config file not found, creating default config");
            resetToDefaults();
            return save();
        }

        if (ll::config::loadConfig(config, configPath)) {
            logger.info("Configuration loaded successfully from {}", configPath.string());

            // 验证配置
            if (!validateConfig()) {
                logger.warn("Configuration validation failed, using defaults");
                resetToDefaults();
                save();
            }

            return true;
        } else {
            logger.error("Failed to load configuration from {}", configPath.string());
            resetToDefaults();
            return false;
        }
    } catch (const std::exception& e) {
        logger.error("Exception while loading config: {}", e.what());
        resetToDefaults();
        return false;
    }
}

bool ConfigManager::save() {
    auto& mod    = ll::mod::NativeMod::current();
    auto& logger = mod.getLogger();

    try {
        // 确保配置目录存在
        std::filesystem::create_directories(configPath.parent_path());

        if (ll::config::saveConfig(config, configPath)) {
            logger.info("Configuration saved successfully to {}", configPath.string());
            return true;
        } else {
            logger.error("Failed to save configuration to {}", configPath.string());
            return false;
        }
    } catch (const std::exception& e) {
        logger.error("Exception while saving config: {}", e.what());
        return false;
    }
}

bool ConfigManager::reload() {
    auto& mod = ll::mod::NativeMod::current();
    mod.getLogger().info("Reloading configuration...");

    resetToDefaults(); // 重置为默认值
    return load();     // 重新加载
}

bool ConfigManager::isFeatureEnabled(const std::string& name) const { return config.isFeatureEnabled(name); }

bool ConfigManager::enableFeature(const std::string& name) {
    bool result = config.enableFeature(name);
    if (result) {
        save(); // 自动保存配置更改
    }
    return result;
}

bool ConfigManager::disableFeature(const std::string& name) {
    bool result = config.disableFeature(name);
    if (result) {
        save(); // 自动保存配置更改
    }
    return result;
}

void ConfigManager::resetToDefaults() {
    config = Config{}; // 重置为默认配置

    // 设置一些常用功能的默认启用状态
    config.features.villageInfo.enabled       = false;
    config.features.profiler.enabled          = false;
    config.features.hudDisplay.enabled        = false;
    config.features.coordinateDisplay.enabled = false;
}

bool ConfigManager::validateConfig() const {
    // 基础验证
    if (config.version <= 0) return false;
    if (config.general.language.empty()) return false;
    if (config.commands.prefix.empty()) return false;
    if (config.commands.permissionLevel < 0 || config.commands.permissionLevel > 4) return false;

    return true;
}

} // namespace carpet_mod_for_ll