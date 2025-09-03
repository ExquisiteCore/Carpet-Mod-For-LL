#include "TickModule.h"
#include <ll/api/i18n/I18n.h>
#include <ll/api/mod/NativeMod.h>
#include <numeric>


namespace carpet_mod_for_ll {

using ll::i18n_literals::operator""_tr;

TickModule::TickModule() : BaseModule("TickControl", "carpet.module.tick.description"_tr()) {}

bool TickModule::onEnable() {
    auto mod = ll::mod::NativeMod::current();
    mod->getLogger().info("TickModule enabled");
    // TODO: Hook ServerLevel::tick
    return true;
}

bool TickModule::onDisable() {
    auto mod = ll::mod::NativeMod::current();
    mod->getLogger().info("TickModule disabled");
    resetTick();
    // TODO: Unhook
    return true;
}

void TickModule::freezeTick() {
    if (currentStatus != TickStatus::Frozen) {
        currentStatus = TickStatus::Frozen;
        auto mod      = ll::mod::NativeMod::current();
        mod->getLogger().info("World ticking frozen");
        // TODO: Implement freeze logic
    }
}

void TickModule::resetTick() {
    currentStatus   = TickStatus::Normal;
    speedMultiplier = 1;
    targetTicks     = 0;
    remainingTicks  = 0;
    auto mod        = ll::mod::NativeMod::current();
    mod->getLogger().info("World ticking reset to normal");
    // TODO: Implement reset logic
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
    auto mod       = ll::mod::NativeMod::current();
    mod->getLogger().info("Forwarding {} ticks", ticks);
    // TODO: Implement forward logic
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
    auto mod       = ll::mod::NativeMod::current();
    mod->getLogger().info("Warping {} ticks", ticks);
    // TODO: Implement warp logic (no rendering)
}

void TickModule::accelerateTick(int multiplier) {
    if (multiplier < 2 || multiplier > 100) {
        auto mod = ll::mod::NativeMod::current();
        mod->getLogger().error("Invalid acceleration multiplier: {}", multiplier);
        return;
    }

    currentStatus   = TickStatus::Accelerated;
    speedMultiplier = multiplier;
    auto mod        = ll::mod::NativeMod::current();
    mod->getLogger().info("Accelerating world {}x", multiplier);
    // TODO: Implement acceleration logic
}

void TickModule::slowDownTick(int divider) {
    if (divider < 2 || divider > 100) {
        auto mod = ll::mod::NativeMod::current();
        mod->getLogger().error("Invalid slowdown divider: {}", divider);
        return;
    }

    currentStatus   = TickStatus::SlowedDown;
    speedMultiplier = divider;
    auto mod        = ll::mod::NativeMod::current();
    mod->getLogger().info("Slowing down world by {}", divider);
    // TODO: Implement slowdown logic
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

} // namespace carpet_mod_for_ll