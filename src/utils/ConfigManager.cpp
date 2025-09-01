#include "ConfigManager.h"
#include <ll/api/Config.h>
#include <ll/api/mod/NativeMod.h>


namespace carpet_mod_for_ll {

std::unique_ptr<ConfigManager> ConfigManager::instance = nullptr;

ConfigManager::ConfigManager() {
    auto mod   = ll::mod::NativeMod::current();
    configPath = mod->getConfigDir() / "config.json";
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
    auto mod = ll::mod::NativeMod::current();

    // Load or initialize configurations
    if (!ll::config::loadConfig(config, configPath)) {
        mod->getLogger().warn("Cannot load configurations from {}", configPath.string());
        mod->getLogger().info("Saving default configurations");
        
        resetToDefaults();
        
        if (!ll::config::saveConfig(config, configPath)) {
            mod->getLogger().error("Cannot save default configurations to {}", configPath.string());
            return false;
        }
    } else {
        mod->getLogger().info("Configuration loaded successfully from {}", configPath.string());
        
        // 验证配置
        if (!validateConfig()) {
            mod->getLogger().warn("Configuration validation failed, using defaults");
            resetToDefaults();
            save();
        }
    }

    return true;
}

bool ConfigManager::save() {
    auto mod = ll::mod::NativeMod::current();

    // 确保配置目录存在
    std::filesystem::create_directories(configPath.parent_path());

    if (ll::config::saveConfig(config, configPath)) {
        mod->getLogger().info("Configuration saved successfully to {}", configPath.string());
        return true;
    } else {
        mod->getLogger().error("Failed to save configuration to {}", configPath.string());
        return false;
    }
}

bool ConfigManager::reload() {
    auto mod = ll::mod::NativeMod::current();
    mod->getLogger().info("Reloading configuration...");

    resetToDefaults(); // 重置为默认值
    return load();     // 重新加载
}

// 便捷访问方法（需要根据实际功能实现）
bool ConfigManager::isFeatureEnabled(const std::string& name) const {
    // TODO: 根据实际功能字段实现
    // 例如: if (name == "someFeature") return config.features.someFeature;
    return false;
}

bool ConfigManager::enableFeature(const std::string& name) {
    // TODO: 根据实际功能字段实现
    // 例如: if (name == "someFeature") { config.features.someFeature = true; save(); return true; }
    return false;
}

bool ConfigManager::disableFeature(const std::string& name) {
    // TODO: 根据实际功能字段实现  
    // 例如: if (name == "someFeature") { config.features.someFeature = false; save(); return true; }
    return false;
}

void ConfigManager::resetToDefaults() {
    config = Config{}; // 重置为默认配置
    
    // TODO: 设置功能的默认启用状态
    // config.features.someFeature = false;
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