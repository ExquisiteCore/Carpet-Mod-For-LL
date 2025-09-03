#include "ProfilerModule.h"
#include "TickHookManager.h"
#include <ll/api/i18n/I18n.h>
#include <ll/api/mod/NativeMod.h>
#include <algorithm>

namespace carpet_mod_for_ll {

using ll::i18n_literals::operator""_tr;

ProfilerModule::ProfilerModule() 
    : BaseModule("Profiler", "carpet.module.profiler.description"_tr()) {
}

bool ProfilerModule::onEnable() {
    auto mod = ll::mod::NativeMod::current();
    mod->getLogger().info("ProfilerModule enabled");
    
    // Enable profiling in TickHookManager
    TickHookManager::enableProfiling(true);
    
    return true;
}

bool ProfilerModule::onDisable() {
    auto mod = ll::mod::NativeMod::current();
    mod->getLogger().info("ProfilerModule disabled");
    
    stopProfiling();
    
    // Disable profiling in TickHookManager
    TickHookManager::enableProfiling(false);
    
    return true;
}

void ProfilerModule::startProfiling(int ticks, const std::string& mode) {
    if (isProfiling_) {
        auto mod = ll::mod::NativeMod::current();
        mod->getLogger().warn("Profiling already in progress");
        return;
    }
    
    if (ticks <= 0 || ticks > 12000) {
        auto mod = ll::mod::NativeMod::current();
        mod->getLogger().error("Invalid tick count: {} (must be 1-12000)", ticks);
        return;
    }
    
    resetProfileData();
    targetTicks = ticks;
    currentTick = 0;
    isProfiling_ = true;
    
    auto mod = ll::mod::NativeMod::current();
    
    if (mode == "normal") {
        currentMode = ProfileMode::Normal;
        TickHookManager::setProfilingMode(0);
        mod->getLogger().info("Starting normal profiling for {} ticks", ticks);
    } else if (mode == "chunk") {
        currentMode = ProfileMode::Chunk;
        TickHookManager::setProfilingMode(1);
        mod->getLogger().info("Starting chunk profiling for {} ticks", ticks);
    } else if (mode == "entity") {
        currentMode = ProfileMode::Entity;
        TickHookManager::setProfilingMode(2);
        mod->getLogger().info("Starting entity profiling for {} ticks", ticks);
    } else if (mode == "pt" || mode == "pendingtick") {
        currentMode = ProfileMode::PendingTick;
        TickHookManager::setProfilingMode(3);
        mod->getLogger().info("Starting pending tick profiling for {} ticks", ticks);
    } else if (mode == "mspt") {
        currentMode = ProfileMode::MSPT;
        TickHookManager::setProfilingMode(4);
        mod->getLogger().info("Starting MSPT profiling for {} ticks", ticks);
    } else {
        currentMode = ProfileMode::Normal;
        TickHookManager::setProfilingMode(0);
        mod->getLogger().info("Starting normal profiling for {} ticks (default)", ticks);
    }
}

void ProfilerModule::stopProfiling() {
    if (!isProfiling_) {
        return;
    }
    
    isProfiling_ = false;
    auto mod = ll::mod::NativeMod::current();
    mod->getLogger().info("Profiling stopped");
    showProfileResults();
}

void ProfilerModule::showProfileResults() {
    switch (currentMode) {
        case ProfileMode::Normal:
            printNormalResults();
            break;
        case ProfileMode::Chunk:
            printChunkResults();
            break;
        case ProfileMode::Entity:
            printEntityResults();
            break;
        case ProfileMode::PendingTick:
            printPendingTickResults();
            break;
        case ProfileMode::MSPT:
            printMSPTResults();
            break;
        default:
            break;
    }
}

void ProfilerModule::profileNormal(int ticks) {
    startProfiling(ticks, "normal");
}

void ProfilerModule::profileChunk(int ticks) {
    startProfiling(ticks, "chunk");
}

void ProfilerModule::profileEntity(int ticks) {
    startProfiling(ticks, "entity");
}

void ProfilerModule::profilePendingTick(int ticks) {
    startProfiling(ticks, "pt");
}

void ProfilerModule::profileMSPT(int ticks) {
    startProfiling(ticks, "mspt");
}

void ProfilerModule::recordTickTime(std::chrono::microseconds time) {
    if (!isProfiling_) return;
    
    overallData.addSample(time);
    currentTick++;
    
    // Auto-stop when target reached
    if (currentTick >= targetTicks) {
        stopProfiling();
    }
}

void ProfilerModule::recordChunkTime(int chunkX, int chunkZ, std::chrono::microseconds time) {
    if (!isProfiling_ || currentMode != ProfileMode::Chunk) return;
    
    auto pos = std::make_pair(chunkX, chunkZ);
    chunkData.chunkTimes[pos].addSample(time);
}

void ProfilerModule::recordEntityTime(const std::string& entityType, std::chrono::microseconds time) {
    if (!isProfiling_ || currentMode != ProfileMode::Entity) return;
    
    entityData.entityTypes[entityType].addSample(time);
    entityData.totalEntityCount++;
}

void ProfilerModule::recordRedstoneTime(std::chrono::microseconds time) {
    if (!isProfiling_) return;
    
    // Can be used for redstone-specific profiling
    // Currently just logs for debugging
    auto mod = ll::mod::NativeMod::current();
    mod->getLogger().debug("Redstone tick: {} us", time.count());
}

void ProfilerModule::ProfileData::addSample(std::chrono::microseconds time) {
    totalTime += time;
    sampleCount++;
    minTime = std::min(minTime, time);
    maxTime = std::max(maxTime, time);
}

double ProfilerModule::ProfileData::getAverageMs() const {
    if (sampleCount == 0) return 0.0;
    return totalTime.count() / 1000.0 / sampleCount;
}

void ProfilerModule::resetProfileData() {
    overallData = ProfileData{};
    chunkData = ChunkProfileData{};
    entityData = EntityProfileData{};
    pendingTickCounts.clear();
}

void ProfilerModule::printNormalResults() {
    auto mod = ll::mod::NativeMod::current();
    mod->getLogger().info("=== Normal Profile Results ===");
    mod->getLogger().info("Average tick time: {:.2f}ms", overallData.getAverageMs());
    mod->getLogger().info("Min/Max: {:.2f}ms / {:.2f}ms", 
        overallData.minTime.count() / 1000.0, 
        overallData.maxTime.count() / 1000.0);
    mod->getLogger().info("Samples: {}", overallData.sampleCount);
    // TODO: Add more detailed breakdown
}

void ProfilerModule::printChunkResults() {
    auto mod = ll::mod::NativeMod::current();
    mod->getLogger().info("=== Chunk Profile Results ===");
    
    // Sort chunks by average time
    std::vector<std::pair<std::pair<int, int>, double>> sortedChunks;
    for (const auto& [pos, data] : chunkData.chunkTimes) {
        sortedChunks.emplace_back(pos, data.getAverageMs());
    }
    std::sort(sortedChunks.begin(), sortedChunks.end(), 
        [](const auto& a, const auto& b) { return a.second > b.second; });
    
    mod->getLogger().info("Top 10 chunks by time:");
    for (size_t i = 0; i < std::min(size_t(10), sortedChunks.size()); i++) {
        const auto& [pos, avgTime] = sortedChunks[i];
        mod->getLogger().info("  [{}, {}]: {:.2f}ms", pos.first, pos.second, avgTime);
    }
    
    mod->getLogger().info("Random tick: {:.2f}ms", chunkData.randomTickTime.getAverageMs());
    mod->getLogger().info("Block entities: {:.2f}ms", chunkData.blockEntityTickTime.getAverageMs());
    mod->getLogger().info("Pending ticks: {:.2f}ms", chunkData.pendingTickTime.getAverageMs());
}

void ProfilerModule::printEntityResults() {
    auto mod = ll::mod::NativeMod::current();
    mod->getLogger().info("=== Entity Profile Results ===");
    
    // Sort entities by total time
    std::vector<std::pair<std::string, double>> sortedEntities;
    for (const auto& [type, data] : entityData.entityTypes) {
        sortedEntities.emplace_back(type, data.getAverageMs() * data.sampleCount);
    }
    std::sort(sortedEntities.begin(), sortedEntities.end(),
        [](const auto& a, const auto& b) { return a.second > b.second; });
    
    mod->getLogger().info("Top entities by total time:");
    for (size_t i = 0; i < std::min(size_t(10), sortedEntities.size()); i++) {
        const auto& [type, totalTime] = sortedEntities[i];
        auto& data = entityData.entityTypes[type];
        mod->getLogger().info("  {}: {:.2f}ms total, {:.3f}ms avg ({})", 
            type, totalTime, data.getAverageMs(), data.sampleCount);
    }
    mod->getLogger().info("Total entities: {}", entityData.totalEntityCount);
}

void ProfilerModule::printPendingTickResults() {
    auto mod = ll::mod::NativeMod::current();
    mod->getLogger().info("=== Pending Tick Profile Results ===");
    
    // Sort chunks by pending tick count
    std::vector<std::pair<std::pair<int, int>, size_t>> sortedChunks;
    for (const auto& [pos, count] : pendingTickCounts) {
        sortedChunks.emplace_back(pos, count);
    }
    std::sort(sortedChunks.begin(), sortedChunks.end(),
        [](const auto& a, const auto& b) { return a.second > b.second; });
    
    mod->getLogger().info("Chunks with most pending ticks:");
    for (size_t i = 0; i < std::min(size_t(10), sortedChunks.size()); i++) {
        const auto& [pos, count] = sortedChunks[i];
        mod->getLogger().info("  [{}, {}]: {} pending ticks", pos.first, pos.second, count);
    }
}

void ProfilerModule::printMSPTResults() {
    auto mod = ll::mod::NativeMod::current();
    mod->getLogger().info("=== MSPT Profile Results ===");
    mod->getLogger().info("Average MSPT: {:.2f}ms", overallData.getAverageMs());
    mod->getLogger().info("Min MSPT: {:.2f}ms", overallData.minTime.count() / 1000.0);
    mod->getLogger().info("Max MSPT: {:.2f}ms", overallData.maxTime.count() / 1000.0);
    double avgTPS = 1000.0 / overallData.getAverageMs();
    mod->getLogger().info("Average TPS: {:.1f}", std::min(20.0, avgTPS));
}

} // namespace carpet_mod_for_ll