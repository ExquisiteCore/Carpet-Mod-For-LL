#include "ProfilerModule.h"
#include <ll/api/mod/NativeMod.h>
#include <ll/api/service/Bedrock.h>
#include <numeric>
#include <sstream>
#include <iomanip>

namespace carpet_mod_for_ll {

ProfilerModule::ProfilerModule()
    : BaseModule("profiler", "Server performance profiler and monitoring") {}

bool ProfilerModule::onEnable() {
    try {
        startProfiling();
        
        auto& mod = ll::mod::NativeMod::current();
        mod.getLogger().info("Profiler module enabled - monitoring server performance");
        
        return true;
    } catch (const std::exception& e) {
        setError("Failed to enable profiler module: " + std::string(e.what()));
        return false;
    }
}

bool ProfilerModule::onDisable() {
    try {
        stopProfiling();
        
        // 清空历史数据
        tpsHistory.clear();
        msptHistory.clear();
        
        auto& mod = ll::mod::NativeMod::current();
        mod.getLogger().info("Profiler module disabled");
        
        return true;
    } catch (const std::exception& e) {
        setError("Failed to disable profiler module: " + std::string(e.what()));
        return false;
    }
}

void ProfilerModule::onTick() {
    if (!isRunning) return;
    
    try {
        updatePerformanceData();
        
        // 每20个tick收集一次详细统计（1秒）
        static int tickCounter = 0;
        tickCounter++;
        
        if (tickCounter % 20 == 0) {
            collectEntityStats();
            collectChunkStats();
            collectMemoryStats();
        }
    } catch (const std::exception& e) {
        auto& mod = ll::mod::NativeMod::current();
        mod.getLogger().error("Error in profiler tick: {}", e.what());
    }
}

void ProfilerModule::startProfiling() {
    isRunning = true;
    lastTick = std::chrono::steady_clock::now();
    
    // 初始化历史数据容器
    tpsHistory.reserve(historySize);
    msptHistory.reserve(historySize);
}

void ProfilerModule::stopProfiling() {
    isRunning = false;
}

void ProfilerModule::updatePerformanceData() {
    auto now = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastTick);
    
    // 计算当前MSPT
    double currentMSPT = duration.count();
    perfData.currentMSPT = currentMSPT;
    
    // 计算当前TPS
    if (currentMSPT > 0) {
        perfData.currentTPS = std::min(20.0, 1000.0 / currentMSPT);
    } else {
        perfData.currentTPS = 20.0;
    }
    
    // 更新历史数据
    if (tpsHistory.size() >= historySize) {
        tpsHistory.erase(tpsHistory.begin());
        msptHistory.erase(msptHistory.begin());
    }
    
    tpsHistory.push_back(perfData.currentTPS);
    msptHistory.push_back(perfData.currentMSPT);
    
    // 计算平均值
    if (!tpsHistory.empty()) {
        perfData.averageTPS = std::accumulate(tpsHistory.begin(), tpsHistory.end(), 0.0) / tpsHistory.size();
        perfData.averageMSPT = std::accumulate(msptHistory.begin(), msptHistory.end(), 0.0) / msptHistory.size();
    }
    
    lastTick = now;
}

void ProfilerModule::collectEntityStats() {
    // TODO: 实现实际的实体统计逻辑
    // 这里需要使用 LeviLamina 的 API 来访问世界数据
    
    /*
    perfData.totalEntities = 0;
    perfData.entityCounts.clear();
    
    auto* level = ll::service::getLevel();
    if (!level) return;
    
    for (auto& dimension : level->getDimensions()) {
        auto& entityList = dimension->getEntities();
        
        for (auto& entity : entityList) {
            if (!entity) continue;
            
            perfData.totalEntities++;
            
            std::string entityType = entity->getTypeName();
            perfData.entityCounts[entityType]++;
        }
    }
    */
    
    // 示例数据
    perfData.totalEntities = 150;
    perfData.entityCounts["minecraft:zombie"] = 20;
    perfData.entityCounts["minecraft:cow"] = 15;
    perfData.entityCounts["minecraft:chicken"] = 30;
    perfData.entityCounts["minecraft:villager"] = 5;
    perfData.entityCounts["minecraft:player"] = 2;
}

void ProfilerModule::collectChunkStats() {
    // TODO: 实现实际的区块统计逻辑
    
    /*
    perfData.loadedChunks = 0;
    perfData.activeChunks = 0;
    
    auto* level = ll::service::getLevel();
    if (!level) return;
    
    for (auto& dimension : level->getDimensions()) {
        auto* chunkSource = dimension->getChunkSource();
        if (!chunkSource) continue;
        
        // 获取加载的区块数量
        perfData.loadedChunks += chunkSource->getLoadedChunksCount();
        
        // 获取活跃区块数量（有玩家附近的区块）
        perfData.activeChunks += chunkSource->getActiveChunksCount();
    }
    */
    
    // 示例数据
    perfData.loadedChunks = 256;
    perfData.activeChunks = 64;
}

void ProfilerModule::collectMemoryStats() {
    // TODO: 实现实际的内存统计逻辑
    // 这里可能需要使用系统API或者LeviLamina提供的内存统计
    
    // 示例数据
    perfData.memoryUsed = 1024 * 1024 * 512;  // 512MB
    perfData.memoryTotal = 1024 * 1024 * 2048; // 2GB
}

std::string ProfilerModule::generateReport() const {
    std::stringstream ss;
    
    ss << "========== Server Performance Report ==========\n";
    ss << std::fixed << std::setprecision(2);
    
    // TPS信息
    ss << "TPS Information:\n";
    ss << "  Current TPS: " << perfData.currentTPS << "\n";
    ss << "  Average TPS: " << perfData.averageTPS << "\n";
    ss << "  Current MSPT: " << perfData.currentMSPT << " ms\n";
    ss << "  Average MSPT: " << perfData.averageMSPT << " ms\n\n";
    
    // 内存信息
    ss << "Memory Usage:\n";
    ss << "  Used: " << (perfData.memoryUsed / (1024 * 1024)) << " MB\n";
    ss << "  Total: " << (perfData.memoryTotal / (1024 * 1024)) << " MB\n";
    ss << "  Usage: " << (perfData.memoryUsed * 100.0 / perfData.memoryTotal) << "%\n\n";
    
    // 世界信息
    ss << "World Statistics:\n";
    ss << "  Loaded Chunks: " << perfData.loadedChunks << "\n";
    ss << "  Active Chunks: " << perfData.activeChunks << "\n";
    ss << "  Total Entities: " << perfData.totalEntities << "\n";
    ss << "  Online Players: " << perfData.onlinePlayers << "\n\n";
    
    // 实体统计
    if (!perfData.entityCounts.empty()) {
        ss << "Entity Breakdown:\n";
        for (const auto& pair : perfData.entityCounts) {
            ss << "  " << pair.first << ": " << pair.second << "\n";
        }
    }
    
    return ss.str();
}

std::string ProfilerModule::generateSimpleStatus() const {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(1);
    
    // 简化的状态信息
    ss << "TPS: " << perfData.currentTPS;
    ss << " | MSPT: " << perfData.currentMSPT << "ms";
    ss << " | Memory: " << (perfData.memoryUsed / (1024 * 1024)) << "MB";
    ss << " | Entities: " << perfData.totalEntities;
    ss << " | Chunks: " << perfData.loadedChunks;
    
    return ss.str();
}

void ProfilerModule::setHistorySize(int size) {
    if (size <= 0 || size > 1000) return; // 限制合理范围
    
    historySize = size;
    
    // 调整现有历史数据大小
    while (tpsHistory.size() > historySize) {
        tpsHistory.erase(tpsHistory.begin());
        msptHistory.erase(msptHistory.begin());
    }
    
    tpsHistory.reserve(historySize);
    msptHistory.reserve(historySize);
}

} // namespace carpet_mod_for_ll