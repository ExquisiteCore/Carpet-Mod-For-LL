#include "TickModule.h"
#include "TickHookManager.h"
#include "ProfilerModule.h"
#include "../functions/BaseModule.h"
#include <ll/api/i18n/I18n.h>
#include <ll/api/mod/NativeMod.h>
#include <numeric>


namespace carpet_mod_for_ll {

using ll::i18n_literals::operator""_tr;

TickModule::TickModule() : BaseModule("TickControl", "carpet.module.tick.description"_tr()) {}

bool TickModule::onEnable() {
    auto mod = ll::mod::NativeMod::current();
    mod->getLogger().info("TickModule enabled");
    
    // Initialize TickHookManager
    TickHookManager::initialize();
    TickHookManager::setTickEnabled(true);
    
    return true;
}

bool TickModule::onDisable() {
    auto mod = ll::mod::NativeMod::current();
    mod->getLogger().info("TickModule disabled");
    
    resetTick();
    
    // Cleanup TickHookManager
    TickHookManager::setTickEnabled(false);
    TickHookManager::cleanup();
    
    return true;
}

void TickModule::freezeTick() {
    if (currentStatus != TickStatus::Frozen) {
        currentStatus = TickStatus::Frozen;
        TickHookManager::setFrozen(true);
        
        auto mod = ll::mod::NativeMod::current();
        mod->getLogger().info("World ticking frozen");
    }
}

void TickModule::resetTick() {
    currentStatus   = TickStatus::Normal;
    speedMultiplier = 1;
    targetTicks     = 0;
    remainingTicks  = 0;
    
    // Reset all states in TickHookManager
    TickHookManager::setFrozen(false);
    TickHookManager::setSpeedMultiplier(1);
    TickHookManager::setSlowdownDivider(1);
    TickHookManager::setForwarding(false);
    TickHookManager::setWarping(false);
    
    auto mod = ll::mod::NativeMod::current();
    mod->getLogger().info("World ticking reset to normal");
}

void TickModule::forwardTick(int ticks) {
    if (ticks <= 0 || ticks > 1000000) {
        auto mod = ll::mod::NativeMod::current();
        mod->getLogger().error("Invalid tick count: {}", ticks);
        return;
    }

    currentStatus  = TickStatus::Forwarding;
    targetTicks    = ticks;
    remainingTicks = ticks;
    
    TickHookManager::setForwarding(true, ticks);
    
    auto mod = ll::mod::NativeMod::current();
    mod->getLogger().info("Forwarding {} ticks", ticks);
}

void TickModule::warpTick(int ticks) {
    if (ticks <= 0 || ticks > 1000000) {
        auto mod = ll::mod::NativeMod::current();
        mod->getLogger().error("Invalid tick count: {}", ticks);
        return;
    }

    currentStatus  = TickStatus::Warping;
    targetTicks    = ticks;
    remainingTicks = ticks;
    
    TickHookManager::setWarping(true, ticks);
    
    auto mod = ll::mod::NativeMod::current();
    mod->getLogger().info("Warping {} ticks", ticks);
}

void TickModule::accelerateTick(int multiplier) {
    if (multiplier < 2 || multiplier > 100) {
        auto mod = ll::mod::NativeMod::current();
        mod->getLogger().error("Invalid acceleration multiplier: {}", multiplier);
        return;
    }

    currentStatus   = TickStatus::Accelerated;
    speedMultiplier = multiplier;
    
    TickHookManager::setSpeedMultiplier(multiplier);
    
    auto mod = ll::mod::NativeMod::current();
    mod->getLogger().info("Accelerating world {}x", multiplier);
}

void TickModule::slowDownTick(int divider) {
    if (divider < 2 || divider > 100) {
        auto mod = ll::mod::NativeMod::current();
        mod->getLogger().error("Invalid slowdown divider: {}", divider);
        return;
    }

    currentStatus   = TickStatus::SlowedDown;
    speedMultiplier = divider;
    
    TickHookManager::setSlowdownDivider(divider);
    
    auto mod = ll::mod::NativeMod::current();
    mod->getLogger().info("Slowing down world by {}", divider);
}

void TickModule::queryTickStatus() {
    std::string statusStr;
    switch (currentStatus) {
    case TickStatus::Normal:
        statusStr = "Normal";
        break;
    case TickStatus::Frozen:
        statusStr = "Frozen";
        break;
    case TickStatus::Forwarding:
        statusStr = fmt::format("Forwarding ({}/{})", targetTicks - remainingTicks, targetTicks);
        break;
    case TickStatus::Warping:
        statusStr = fmt::format("Warping ({}/{})", targetTicks - remainingTicks, targetTicks);
        break;
    case TickStatus::Accelerated:
        statusStr = fmt::format("Accelerated ({}x)", speedMultiplier);
        break;
    case TickStatus::SlowedDown:
        statusStr = fmt::format("Slowed Down (1/{})", speedMultiplier);
        break;
    }

    auto mod = ll::mod::NativeMod::current();
    mod->getLogger().info("Tick status: {}", statusStr);
}

void TickModule::showMSPT() {
    double avgMSPT = getAverageMSPT();
    double tps     = getTPS();
    auto   mod     = ll::mod::NativeMod::current();
    mod->getLogger().info("MSPT: {:.2f}ms, TPS: {:.1f}", avgMSPT, tps);
}

void TickModule::recordMSPT(double mspt) {
    msptHistory.push_back(mspt);
    if (msptHistory.size() > MAX_MSPT_HISTORY) {
        msptHistory.pop_front();
    }
}

double TickModule::getAverageMSPT() const {
    if (msptHistory.empty()) {
        return 50.0; // Default 50ms (20 TPS)
    }
    double sum = std::accumulate(msptHistory.begin(), msptHistory.end(), 0.0);
    return sum / msptHistory.size();
}

double TickModule::getTPS() const {
    double avgMSPT = getAverageMSPT();
    if (avgMSPT <= 0) {
        return 20.0;
    }
    return std::min(20.0, 1000.0 / avgMSPT);
}

bool TickModule::shouldTickThisFrame() {
    // For slowdown mode, only tick every N frames
    static int frameCounter = 0;
    if (currentStatus == TickStatus::SlowedDown) {
        frameCounter++;
        if (frameCounter >= speedMultiplier) {
            frameCounter = 0;
            return true;
        }
        return false;
    }
    return true;
}

void TickModule::onTickExecuted() {
    // 更新remaining ticks for forward/warp modes
    if (currentStatus == TickStatus::Forwarding || currentStatus == TickStatus::Warping) {
        remainingTicks--;
        if (remainingTicks <= 0) {
            auto mod = ll::mod::NativeMod::current();
            mod->getLogger().info("Completed {} ticks", targetTicks);
            resetTick();
        }
    }
    
    // 同时更新MSPT历史记录（如果有ProfilerModule提供的数据）
    auto* profilerModule = dynamic_cast<ProfilerModule*>(
        ModuleManager::getModule("Profiler")
    );
    if (profilerModule && profilerModule->isProfiling()) {
        // ProfilerModule会自动记录MSPT数据
    }
}

} // namespace carpet_mod_for_ll