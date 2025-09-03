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
        
        // Tick控制功能设置
        struct TickControl {
            bool enabled = true;
            int maxForwardTicks = 1000000; // 最大前进tick数
            int maxWarpTicks = 1000000; // 最大warp tick数
            int maxAcceleration = 100; // 最大加速倍数
            int maxSlowdown = 100; // 最大减速倍数
            bool allowWarp = true; // 是否允许warp模式
            int msptHistorySize = 100; // MSPT历史记录大小
        } tickControl;
        
        // 性能分析器设置
        struct Profiler {
            bool enabled = true;
            int maxProfilingTicks = 12000; // 最大分析tick数
            int defaultProfilingTicks = 100; // 默认分析tick数
            bool autoStop = true; // 达到目标tick后自动停止
            bool showDetailedResults = true; // 显示详细结果
            int topChunksToShow = 10; // 显示前N个区块
            int topEntitiesToShow = 10; // 显示前N个实体类型
        } profiler;
    } features;
};

} // namespace carpet_mod_for_ll