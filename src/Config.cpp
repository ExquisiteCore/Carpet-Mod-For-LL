#include "Config.h"
#include <unordered_map>
#include <functional>

namespace carpet_mod_for_ll {

// 功能名称到配置指针的映射
static std::unordered_map<std::string, std::function<FeatureConfig*(Config*)>> featureMap = {
    // 村庄相关
    {"villageInfo", [](Config* c) { return &c->features.villageInfo; }},
    {"villageVisualize", [](Config* c) { return &c->features.villageVisualize; }},
    
    // 性能分析
    {"profiler", [](Config* c) { return &c->features.profiler; }},
    {"tpsMonitor", [](Config* c) { return &c->features.tpsMonitor; }},
    
    // 假玩家
    {"fakePlayers", [](Config* c) { return &c->features.fakePlayers; }},
    {"playerManager", [](Config* c) { return &c->features.playerManager; }},
    
    // 世界控制
    {"tickControl", [](Config* c) { return &c->features.tickControl; }},
    {"chunkLoader", [](Config* c) { return &c->features.chunkLoader; }},
    
    // 技术工具
    {"redstoneTools", [](Config* c) { return &c->features.redstoneTools; }},
    {"blockInfo", [](Config* c) { return &c->features.blockInfo; }},
    {"entityInfo", [](Config* c) { return &c->features.entityInfo; }},
    
    // HUD显示
    {"hudDisplay", [](Config* c) { return &c->features.hudDisplay; }},
    {"coordinateDisplay", [](Config* c) { return &c->features.coordinateDisplay; }},
    
    // 实用工具
    {"hopperCounter", [](Config* c) { return &c->features.hopperCounter; }},
    {"slimeChunk", [](Config* c) { return &c->features.slimeChunk; }},
    {"spawnAnalyzer", [](Config* c) { return &c->features.spawnAnalyzer; }},
    {"distanceTools", [](Config* c) { return &c->features.distanceTools; }}
};

FeatureConfig* Config::getFeature(const std::string& name) {
    auto it = featureMap.find(name);
    if (it != featureMap.end()) {
        return it->second(this);
    }
    return nullptr;
}

const FeatureConfig* Config::getFeature(const std::string& name) const {
    auto it = featureMap.find(name);
    if (it != featureMap.end()) {
        return it->second(const_cast<Config*>(this));
    }
    return nullptr;
}

std::vector<std::string> Config::getAllFeatureNames() const {
    std::vector<std::string> names;
    names.reserve(featureMap.size());
    
    for (const auto& pair : featureMap) {
        names.push_back(pair.first);
    }
    
    return names;
}

bool Config::enableFeature(const std::string& name) {
    auto* feature = getFeature(name);
    if (feature) {
        feature->enabled = true;
        return true;
    }
    return false;
}

bool Config::disableFeature(const std::string& name) {
    auto* feature = getFeature(name);
    if (feature) {
        feature->enabled = false;
        return true;
    }
    return false;
}

bool Config::isFeatureEnabled(const std::string& name) const {
    const auto* feature = getFeature(name);
    return feature && feature->enabled;
}

} // namespace carpet_mod_for_ll