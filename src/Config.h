#pragma once

#include <string>
#include <unordered_map>
#include <vector>

namespace carpet_mod_for_ll {

// 功能配置结构
struct FeatureConfig {
    bool enabled = false;
    std::string description;
    std::vector<std::string> permissions;
    std::unordered_map<std::string, std::string> options;
};

// 主配置结构
struct Config {
    int version = 1;
    
    // 基础设置
    struct General {
        std::string language = "en_US";
        bool enableDebugLogging = false;
        bool enableCommandLogging = true;
        int maxCacheSize = 1000;
    } general;
    
    // 命令设置
    struct Commands {
        std::string prefix = "carpet";
        int permissionLevel = 2;
        bool enableAutoComplete = true;
        bool enableCommandHistory = true;
    } commands;
    
    // 功能设置 - 待实现的 Carpet 功能
    struct Features {
        // TODO: 添加 Carpet 功能配置
        // 示例:
        // FeatureConfig someFeature{"Feature description"};
    } features;
    
    // 获取功能配置的便捷方法
    FeatureConfig* getFeature(const std::string& name);
    const FeatureConfig* getFeature(const std::string& name) const;
    
    // 获取所有功能名称
    std::vector<std::string> getAllFeatureNames() const;
    
    // 启用/禁用功能
    bool enableFeature(const std::string& name);
    bool disableFeature(const std::string& name);
    bool isFeatureEnabled(const std::string& name) const;
};

} // namespace carpet_mod_for_ll