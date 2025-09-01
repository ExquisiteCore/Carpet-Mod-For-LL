#include "VillageModule.h"
#include <ll/api/mod/NativeMod.h>

namespace carpet_mod_for_ll {

VillageModule::VillageModule() 
    : BaseModule("villageInfo", "Display village information and boundaries") {}

bool VillageModule::onEnable() {
    try {
        auto& mod = ll::mod::NativeMod::current();
        mod.getLogger().info("Village module enabled - scanning for villages...");
        
        // 初始化村庄数据
        updateVillageInfo();
        
        return true;
    } catch (const std::exception& e) {
        setError("Failed to enable village module: " + std::string(e.what()));
        return false;
    }
}

bool VillageModule::onDisable() {
    try {
        // 隐藏所有显示
        displayVillageBoundaries(false);
        displayVillageInfo(false);
        
        // 清空村庄数据
        villages.clear();
        
        auto& mod = ll::mod::NativeMod::current();
        mod.getLogger().info("Village module disabled");
        
        return true;
    } catch (const std::exception& e) {
        setError("Failed to disable village module: " + std::string(e.what()));
        return false;
    }
}

void VillageModule::onTick() {
    static int tickCounter = 0;
    tickCounter++;
    
    // 每100个tick更新一次村庄信息（5秒）
    if (tickCounter % 100 == 0) {
        updateVillageInfo();
    }
}

std::vector<VillageInfo> VillageModule::scanVillages() {
    std::vector<VillageInfo> foundVillages;
    
    // TODO: 实现实际的村庄扫描逻辑
    // 这里需要使用 LeviLamina 的 API 来访问世界数据
    // 目前只是示例代码
    
    /*
    auto* level = ll::service::getLevel();
    if (!level) return foundVillages;
    
    // 扫描所有维度的村庄
    for (auto& dim : level->getDimensions()) {
        // 获取村庄管理器
        auto* villageManager = dim->getVillageManager();
        if (!villageManager) continue;
        
        // 遍历所有村庄
        for (auto& village : villageManager->getVillages()) {
            VillageInfo info;
            info.name = "Village_" + std::to_string(village.getId());
            info.center = Position(village.getCenter().x, village.getCenter().y, village.getCenter().z);
            info.population = village.getPopulation();
            info.beds = village.getBedCount();
            info.workstations = village.getWorkstationCount();
            info.hasIronGolem = village.hasIronGolem();
            
            foundVillages.push_back(info);
        }
    }
    */
    
    // 示例数据
    VillageInfo exampleVillage;
    exampleVillage.name = "Example Village";
    exampleVillage.center = Position(100, 64, 200);
    exampleVillage.population = 5;
    exampleVillage.beds = 8;
    exampleVillage.workstations = 3;
    exampleVillage.hasIronGolem = false;
    foundVillages.push_back(exampleVillage);
    
    return foundVillages;
}

void VillageModule::updateVillageInfo() {
    villages = scanVillages();
    
    auto& mod = ll::mod::NativeMod::current();
    mod.getLogger().debug("Found {} villages", villages.size());
}

void VillageModule::displayVillageBoundaries(bool show) {
    showBoundaries = show;
    
    if (show) {
        // TODO: 实现边界显示逻辑
        // 使用粒子效果或其他方式显示村庄边界
        auto& mod = ll::mod::NativeMod::current();
        mod.getLogger().info("Village boundaries display enabled");
    } else {
        // TODO: 清除边界显示
        auto& mod = ll::mod::NativeMod::current();
        mod.getLogger().info("Village boundaries display disabled");
    }
}

void VillageModule::displayVillageInfo(bool show) {
    showInfo = show;
    
    if (show) {
        // TODO: 实现信息显示逻辑
        // 可以使用HUD或聊天消息显示村庄信息
        auto& mod = ll::mod::NativeMod::current();
        mod.getLogger().info("Village info display enabled");
    } else {
        // TODO: 清除信息显示
        auto& mod = ll::mod::NativeMod::current();
        mod.getLogger().info("Village info display disabled");
    }
}

} // namespace carpet_mod_for_ll