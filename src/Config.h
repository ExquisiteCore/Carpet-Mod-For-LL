#pragma once

#include <string>

namespace carpet_mod_for_ll {

// 主配置结构
struct Config {
    int version = 1;
    
    // 基础设置
    struct General {
        std::string language = "zh_CN";
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
        // bool someFeature = false;
    } features;
};

} // namespace carpet_mod_for_ll