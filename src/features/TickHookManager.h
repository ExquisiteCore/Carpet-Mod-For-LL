#pragma once

#include <chrono>

namespace carpet_mod_for_ll {

// 前向声明Hook类（使用struct以匹配LL_TYPE_INSTANCE_HOOK宏生成的定义）
struct LevelTickHook;
struct DimensionTickRedstoneHook;

class TickHookManager {
public:
    // 初始化和清理
    static void initialize();
    static void cleanup();
    
    // 状态控制接口（供TickModule调用）
    static void setTickEnabled(bool enabled);
    static void setFrozen(bool frozen);
    static void setSpeedMultiplier(int multiplier);
    static void setSlowdownDivider(int divider);
    static void setForwarding(bool forwarding, int ticks = 0);
    static void setWarping(bool warping, int ticks = 0);
    
    // 查询接口（供TickModule调用）
    static bool isTickExecuting();
    static int getCurrentTickCount();
    
    // 性能数据接口（供ProfilerModule调用）
    static void enableProfiling(bool enable);
    static void setProfilingMode(int mode);
    
    // 回调接口（Hook实现后会调用这些）
    static void onTickStart();
    static void onTickEnd(std::chrono::microseconds elapsed);
    static void onChunkTick(int chunkX, int chunkZ, std::chrono::microseconds elapsed);
    static void onEntityTick(const std::string& entityType, std::chrono::microseconds elapsed);
    static void onRedsttoneTick(std::chrono::microseconds elapsed);
    
    // 友元类声明，允许Hook访问私有成员
    friend struct LevelTickHook;
    friend struct DimensionTickRedstoneHook;
    
private:
    // 内部状态
    static bool tickEnabled;
    static bool frozen;
    static int speedMultiplier;
    static int slowdownDivider;
    static int slowdownCounter;
    static bool forwarding;
    static bool warping;
    static int targetTicks;
    static int remainingTicks;
    
    static bool profilingEnabled;
    static int profilingMode;
    
    // Hook实现
    static void installHooks();
    static void uninstallHooks();
};

} // namespace carpet_mod_for_ll