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

// 便捷访问方法
bool ConfigManager::isFeatureEnabled(const std::string& name) const {
    // 根据功能名称返回对应的启用状态
    if (name == "CactusWrench" || name == "cactus_wrench") {
        return config.features.cactusWrench.enabled;
    }
    if (name == "TickControl" || name == "tick_control") {
        return config.features.tickControl.enabled;
    }
    if (name == "Profiler" || name == "profiler") {
        return config.features.profiler.enabled;
    }

    // 对于未知功能，默认禁用以确保安全
    return false;
}

bool ConfigManager::enableFeature(const std::string& name) {
    auto mod     = ll::mod::NativeMod::current();
    bool changed = false;

    // 更新配置中的功能状态
    if (name == "CactusWrench" || name == "cactus_wrench") {
        if (!config.features.cactusWrench.enabled) {
            config.features.cactusWrench.enabled = true;
            changed                              = true;
        }
    } else if (name == "TickControl" || name == "tick_control") {
        if (!config.features.tickControl.enabled) {
            config.features.tickControl.enabled = true;
            changed                             = true;
        }
    } else if (name == "Profiler" || name == "profiler") {
        if (!config.features.profiler.enabled) {
            config.features.profiler.enabled = true;
            changed                          = true;
        }
    } else {
        mod->getLogger().warn("Unknown feature: {}", name);
        return false;
    }

    if (changed) {
        mod->getLogger().info("Enabled feature: {}", name);
        return save();
    }

    mod->getLogger().info("Feature {} is already enabled", name);
    return true;
}

bool ConfigManager::disableFeature(const std::string& name) {
    auto mod     = ll::mod::NativeMod::current();
    bool changed = false;

    // 更新配置中的功能状态
    if (name == "CactusWrench" || name == "cactus_wrench") {
        if (config.features.cactusWrench.enabled) {
            config.features.cactusWrench.enabled = false;
            changed                              = true;
        }
    } else if (name == "TickControl" || name == "tick_control") {
        if (config.features.tickControl.enabled) {
            config.features.tickControl.enabled = false;
            changed                             = true;
        }
    } else if (name == "Profiler" || name == "profiler") {
        if (config.features.profiler.enabled) {
            config.features.profiler.enabled = false;
            changed                          = true;
        }
    } else {
        mod->getLogger().warn("Unknown feature: {}", name);
        return false;
    }

    if (changed) {
        mod->getLogger().info("Disabled feature: {}", name);
        return save();
    }

    mod->getLogger().info("Feature {} is already disabled", name);
    return true;
}

void ConfigManager::resetToDefaults() {
    config = Config{}; // 重置为默认配置，会自动应用结构体中定义的默认值

    auto mod = ll::mod::NativeMod::current();
    mod->getLogger().info("Configuration reset to defaults");
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