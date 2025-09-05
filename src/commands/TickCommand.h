#pragma once

#include "mc/server/commands/CommandOutput.h"

namespace carpet_mod_for_ll {

// Tick命令管理类
class TickCommand {
public:
    // 注册命令到LeviLamina
    static void registerCommand();

private:
    // 命令处理函数
    static void handleFreeze(CommandOutput& output);
    static void handleReset(CommandOutput& output);
    static void handleForward(CommandOutput& output, int ticks);
    static void handleWarp(CommandOutput& output, int ticks);
    static void handleAcc(CommandOutput& output, int multiplier);
    static void handleSlow(CommandOutput& output, int divider);
    static void handleQuery(CommandOutput& output);
    static void handleMSPT(CommandOutput& output);

    // 获取Tick模块的辅助函数
    static class TickModule* getTickModule();
};

} // namespace carpet_mod_for_ll