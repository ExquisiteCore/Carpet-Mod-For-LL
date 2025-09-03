#pragma once

#include "../functions/BaseModule.h"
#include <chrono>
#include <deque>

namespace carpet_mod_for_ll {

/**
 * Tick控制功能模块
 * 实现世界时间流速控制
 */
class TickModule : public BaseModule {
public:
    TickModule();
    ~TickModule() override = default;

    bool onEnable() override;
    bool onDisable() override;

    // Tick控制功能
    void freezeTick();
    void resetTick();
    void forwardTick(int ticks);
    void warpTick(int ticks);
    void accelerateTick(int multiplier);
    void slowDownTick(int divider);
    void queryTickStatus();
    void showMSPT();

private:
    enum class TickStatus { Normal, Frozen, Forwarding, Warping, Accelerated, SlowedDown };

    TickStatus currentStatus   = TickStatus::Normal;
    int        targetTicks     = 0;
    int        remainingTicks  = 0;
    int        speedMultiplier = 1;

    // MSPT统计
    std::deque<double>      msptHistory;
    static constexpr size_t MAX_MSPT_HISTORY = 100;

    void                 recordMSPT(double mspt);
    [[nodiscard]] double getAverageMSPT() const;
    [[nodiscard]] double getTPS() const;
};

} // namespace carpet_mod_for_ll