#pragma once

#include <string>

namespace carpet_mod_for_ll {

// Tick命令参数结构
struct TickCommandParams {
    enum class Action {
        Freeze,
        Reset,
        Forward,
        Warp,
        Acc,
        Slow,
        Query,
        MSPT
    };
    
    Action action;
    int value = 0;  // for forward/warp/acc/slow commands
};

// Tick命令管理类
class TickCommand {
public:
    // 注册命令到LeviLamina
    static void registerCommand();

private:
    // 命令处理函数
    static void handleFreeze();
    static void handleReset();
    static void handleForward(int ticks);
    static void handleWarp(int ticks);
    static void handleAcc(int multiplier);
    static void handleSlow(int divider);
    static void handleQuery();
    static void handleMSPT();
    
    // 获取Tick模块的辅助函数
    static class TickModule* getTickModule();
};

} // namespace carpet_mod_for_ll