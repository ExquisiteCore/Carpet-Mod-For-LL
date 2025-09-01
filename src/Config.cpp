#include "Config.h"
#include <unordered_map>
#include <functional>

namespace carpet_mod_for_ll {

// 功能名称到配置指针的映射
static std::unordered_map<std::string, std::function<FeatureConfig*(Config*)>> featureMap = {
    // TODO: 添加功能配置映射
    // 示例:
    // {"featureName", [](Config* c) { return &c->features.featureName; }},
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