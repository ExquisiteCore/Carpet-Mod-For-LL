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
    
    // 功能设置 - 参考 trapdoor-ll 的功能模块
    struct Features {
        // 村庄相关
        FeatureConfig villageInfo{"Show village information"};
        FeatureConfig villageVisualize{"Visualize village boundaries"};
        
        // 性能分析
        FeatureConfig profiler{"Server performance profiler"};
        FeatureConfig tpsMonitor{"TPS monitoring"};
        
        // 假玩家
        FeatureConfig fakePlayers{"Fake player system"};
        FeatureConfig playerManager{"Player management tools"};
        
        // 世界控制
        FeatureConfig tickControl{"World tick control"};
        FeatureConfig chunkLoader{"Chunk loading tools"};
        
        // 技术工具
        FeatureConfig redstoneTools{"Redstone debugging tools"};
        FeatureConfig blockInfo{"Block information display"};
        FeatureConfig entityInfo{"Entity information display"};
        
        // HUD显示
        FeatureConfig hudDisplay{"HUD information display"};
        FeatureConfig coordinateDisplay{"Coordinate display"};
        
        // 实用工具
        FeatureConfig hopperCounter{"Hopper item counter"};
        FeatureConfig slimeChunk{"Slime chunk finder"};
        FeatureConfig spawnAnalyzer{"Spawn analysis tools"};
        FeatureConfig distanceTools{"Distance measurement tools"};
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