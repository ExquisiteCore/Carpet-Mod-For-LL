#include "TickHookManager.h"
#include "ProfilerModule.h"
#include "../functions/BaseModule.h"
#include <ll/api/mod/NativeMod.h>
#include <ll/api/memory/Hook.h>
#include <mc/world/level/Level.h>
#include <mc/server/ServerLevel.h>
#include <mc/world/level/dimension/Dimension.h>
#include <chrono>

namespace carpet_mod_for_ll {

// 静态成员初始化
bool TickHookManager::tickEnabled = true;
bool TickHookManager::frozen = false;
int TickHookManager::speedMultiplier = 1;
int TickHookManager::slowdownDivider = 1;
int TickHookManager::slowdownCounter = 0;
bool TickHookManager::forwarding = false;
bool TickHookManager::warping = false;
int TickHookManager::targetTicks = 0;
int TickHookManager::remainingTicks = 0;
bool TickHookManager::profilingEnabled = false;
int TickHookManager::profilingMode = 0;

// Hook定义 - Level::tick()
LL_TYPE_INSTANCE_HOOK(
    LevelTickHook,
    ll::memory::HookPriority::Normal,
    Level,
    &Level::$tick,  // 使用$前缀来hook虚函数
    void
) {
    // 如果tick被禁用或冻结，直接返回
    if (!TickHookManager::tickEnabled || TickHookManager::frozen) {
        return;
    }
    
    // 处理减速模式
    if (TickHookManager::slowdownDivider > 1) {
        TickHookManager::slowdownCounter++;
        if (TickHookManager::slowdownCounter < TickHookManager::slowdownDivider) {
            return; // 跳过这一帧
        }
        TickHookManager::slowdownCounter = 0;
    }
    
    // 记录tick开始时间
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // 处理加速模式 - 执行多次tick
    int tickCount = 1;
    if (TickHookManager::speedMultiplier > 1) {
        tickCount = TickHookManager::speedMultiplier;
    }
    
    // 处理forwarding/warping模式
    if (TickHookManager::forwarding || TickHookManager::warping) {
        if (TickHookManager::warping) {
            // Warp模式：尽可能快地执行多个tick
            tickCount = std::min(TickHookManager::remainingTicks, 100); // 每帧最多100个tick
        }
        // Forward模式使用正常速度
    }
    
    // 执行tick
    for (int i = 0; i < tickCount; i++) {
        origin(); // 调用原始的tick函数
        
        // 更新forwarding/warping计数
        if (TickHookManager::forwarding || TickHookManager::warping) {
            TickHookManager::remainingTicks--;
            if (TickHookManager::remainingTicks <= 0) {
                auto mod = ll::mod::NativeMod::current();
                mod->getLogger().info("Completed {} ticks", TickHookManager::targetTicks);
                TickHookManager::forwarding = false;
                TickHookManager::warping = false;
                break;
            }
        }
    }
    
    // 记录tick结束时间
    auto endTime = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    TickHookManager::onTickEnd(elapsed);
}

// Hook定义 - Dimension::tickRedstone()  
LL_TYPE_INSTANCE_HOOK(
    DimensionTickRedstoneHook,
    ll::memory::HookPriority::Normal,
    Dimension,
    &Dimension::$tickRedstone,  // 使用$前缀来hook虚函数
    void
) {
    // 如果tick被冻结，跳过红石tick
    if (TickHookManager::frozen) {
        return;
    }
    
    auto startTime = std::chrono::high_resolution_clock::now();
    origin();
    auto endTime = std::chrono::high_resolution_clock::now();
    
    if (TickHookManager::profilingEnabled) {
        auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
        TickHookManager::onRedsttoneTick(elapsed);
    }
}

void TickHookManager::initialize() {
    auto mod = ll::mod::NativeMod::current();
    mod->getLogger().info("TickHookManager: Initializing...");
    
    installHooks();
    
    mod->getLogger().info("TickHookManager: Initialized");
}

void TickHookManager::cleanup() {
    auto mod = ll::mod::NativeMod::current();
    mod->getLogger().info("TickHookManager: Cleaning up...");
    
    uninstallHooks();
    
    mod->getLogger().info("TickHookManager: Cleaned up");
}

void TickHookManager::setTickEnabled(bool enabled) {
    tickEnabled = enabled;
    auto mod = ll::mod::NativeMod::current();
    mod->getLogger().debug("TickHookManager: Tick enabled = {}", enabled);
}

void TickHookManager::setFrozen(bool isFrozen) {
    frozen = isFrozen;
    auto mod = ll::mod::NativeMod::current();
    mod->getLogger().debug("TickHookManager: Frozen = {}", isFrozen);
}

void TickHookManager::setSpeedMultiplier(int multiplier) {
    speedMultiplier = multiplier;
    auto mod = ll::mod::NativeMod::current();
    mod->getLogger().debug("TickHookManager: Speed multiplier = {}", multiplier);
}

void TickHookManager::setSlowdownDivider(int divider) {
    slowdownDivider = divider;
    slowdownCounter = 0;
    auto mod = ll::mod::NativeMod::current();
    mod->getLogger().debug("TickHookManager: Slowdown divider = {}", divider);
}

void TickHookManager::setForwarding(bool isForwarding, int ticks) {
    forwarding = isForwarding;
    if (isForwarding) {
        targetTicks = ticks;
        remainingTicks = ticks;
    }
    auto mod = ll::mod::NativeMod::current();
    mod->getLogger().debug("TickHookManager: Forwarding = {}, ticks = {}", isForwarding, ticks);
}

void TickHookManager::setWarping(bool isWarping, int ticks) {
    warping = isWarping;
    if (isWarping) {
        targetTicks = ticks;
        remainingTicks = ticks;
    }
    auto mod = ll::mod::NativeMod::current();
    mod->getLogger().debug("TickHookManager: Warping = {}, ticks = {}", isWarping, ticks);
}

bool TickHookManager::isTickExecuting() {
    // 返回是否正在执行tick
    return !frozen && tickEnabled;
}

int TickHookManager::getCurrentTickCount() {
    return targetTicks - remainingTicks;
}

void TickHookManager::enableProfiling(bool enable) {
    profilingEnabled = enable;
    auto mod = ll::mod::NativeMod::current();
    mod->getLogger().debug("TickHookManager: Profiling enabled = {}", enable);
}

void TickHookManager::setProfilingMode(int mode) {
    profilingMode = mode;
    auto mod = ll::mod::NativeMod::current();
    mod->getLogger().debug("TickHookManager: Profiling mode = {}", mode);
}

void TickHookManager::onTickStart() {
    // 当tick开始时调用
    auto mod = ll::mod::NativeMod::current();
    mod->getLogger().debug("TickHookManager: Tick started");
}

void TickHookManager::onTickEnd(std::chrono::microseconds elapsed) {
    // 记录MSPT数据
    if (profilingEnabled) {
        double mspt = elapsed.count() / 1000.0;
        auto mod = ll::mod::NativeMod::current();
        mod->getLogger().debug("TickHookManager: Tick elapsed = {:.2f}ms", mspt);
        
        // 将数据传递给ProfilerModule
        auto* profilerModule = dynamic_cast<ProfilerModule*>(
            ModuleManager::getModule("Profiler")
        );
        if (profilerModule && profilerModule->isProfiling()) {
            profilerModule->recordTickTime(elapsed);
        }
    }
    
    // 更新forwarding/warping状态已在Hook中处理
}

void TickHookManager::onChunkTick(int chunkX, int chunkZ, std::chrono::microseconds elapsed) {
    // 区块tick性能数据
    if (profilingEnabled && profilingMode == 1) { // Chunk profiling mode
        auto mod = ll::mod::NativeMod::current();
        mod->getLogger().debug("Chunk ({}, {}) tick: {}us", chunkX, chunkZ, elapsed.count());
        
        // 将数据传递给ProfilerModule
        auto* profilerModule = dynamic_cast<ProfilerModule*>(
            ModuleManager::getModule("Profiler")
        );
        if (profilerModule && profilerModule->isProfiling()) {
            profilerModule->recordChunkTime(chunkX, chunkZ, elapsed);
        }
    }
}

void TickHookManager::onEntityTick(const std::string& entityType, std::chrono::microseconds elapsed) {
    // 实体tick性能数据
    if (profilingEnabled && profilingMode == 2) { // Entity profiling mode
        auto mod = ll::mod::NativeMod::current();
        mod->getLogger().debug("Entity {} tick: {}us", entityType, elapsed.count());
        
        // 将数据传递给ProfilerModule
        auto* profilerModule = dynamic_cast<ProfilerModule*>(
            ModuleManager::getModule("Profiler")
        );
        if (profilerModule && profilerModule->isProfiling()) {
            profilerModule->recordEntityTime(entityType, elapsed);
        }
    }
}

void TickHookManager::onRedsttoneTick(std::chrono::microseconds elapsed) {
    // 红石tick性能数据
    if (profilingEnabled) {
        auto mod = ll::mod::NativeMod::current();
        mod->getLogger().debug("Redstone tick: {}us", elapsed.count());
        
        // 将数据传递给ProfilerModule
        auto* profilerModule = dynamic_cast<ProfilerModule*>(
            ModuleManager::getModule("Profiler")
        );
        if (profilerModule && profilerModule->isProfiling()) {
            profilerModule->recordRedstoneTime(elapsed);
        }
    }
}

void TickHookManager::installHooks() {
    auto mod = ll::mod::NativeMod::current();
    mod->getLogger().info("TickHookManager: Installing hooks...");
    
    // 注册Hooks
    LevelTickHook::hook();
    DimensionTickRedstoneHook::hook();
    
    mod->getLogger().info("TickHookManager: Hooks installed");
}

void TickHookManager::uninstallHooks() {
    auto mod = ll::mod::NativeMod::current();
    mod->getLogger().info("TickHookManager: Uninstalling hooks...");
    
    // 卸载Hooks
    LevelTickHook::unhook();
    DimensionTickRedstoneHook::unhook();
    
    mod->getLogger().info("TickHookManager: Hooks uninstalled");
}

} // namespace carpet_mod_for_ll