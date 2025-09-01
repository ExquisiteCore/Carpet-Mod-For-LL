#pragma once

#include "Config.h"
#include <filesystem>
#include <memory>


namespace carpet_mod_for_ll {

class ConfigManager {
private:
    static std::unique_ptr<ConfigManager> instance;
    Config                                config;
    std::filesystem::path                 configPath;

public:
    ConfigManager();
    ~ConfigManager() = default;

    static ConfigManager& getInstance();
    static void           initialize();
    static void           cleanup();

    // 配置文件操作
    bool load();
    bool save();
    bool reload();

    // 配置访问
    Config&                     getConfig() { return config; }
    [[nodiscard]] const Config& getConfig() const { return config; }

    // 便捷访问方法
    [[nodiscard]] bool isFeatureEnabled(const std::string& name) const;
    bool               enableFeature(const std::string& name);
    bool               disableFeature(const std::string& name);

    // 获取配置文件路径
    [[nodiscard]] const std::filesystem::path& getConfigPath() const { return configPath; }

    // 重置为默认配置
    void resetToDefaults();

    // 验证配置
    [[nodiscard]] bool validateConfig() const;
};

// 便捷宏定义
#define CONFIG                   carpet_mod_for_ll::ConfigManager::getInstance().getConfig()
#define IS_FEATURE_ENABLED(name) carpet_mod_for_ll::ConfigManager::getInstance().isFeatureEnabled(name)

} // namespace carpet_mod_for_ll