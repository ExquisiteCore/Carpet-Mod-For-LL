#pragma once

#include "../functions/BaseModule.h"
#include <deque>

namespace carpet_mod_for_ll {

/**
 * Tick控制功能模块
 * 实现世界时间流速控制
 */
class TickModule : public BaseModule {
public:
    enum class TickStatus { Normal, Frozen, Forwarding, Warping, Accelerated, SlowedDown };

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

    // 获取当前状态（供TickHookManager查询）
    [[nodiscard]] TickStatus getCurrentStatus() const { return currentStatus; }
    [[nodiscard]] int        getSpeedMultiplier() const { return speedMultiplier; }
    [[nodiscard]] int        getRemainingTicks() const { return remainingTicks; }
    bool                     shouldTickThisFrame(); // 用于减速模式
    void                     onTickExecuted();      // 每执行一个tick后调用

private:
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