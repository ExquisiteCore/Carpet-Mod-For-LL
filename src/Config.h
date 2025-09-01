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
    
    // 功能设置 - Carpet 功能配置
    struct Features {
        // 仙人掌扳手设置
        struct CactusWrench {
            bool enabled = true;
            bool preventDefaultAction = false; // 是否阻止默认的物品使用行为
            std::string targetCommand = "/carpet counter print {bx} {by} {bz}"; // 执行的命令
        } cactusWrench;
        
        // TODO: 添加其他 Carpet 功能配置
        // 示例:
        // bool someFeature = false;
    } features;
};

} // namespace carpet_mod_for_ll