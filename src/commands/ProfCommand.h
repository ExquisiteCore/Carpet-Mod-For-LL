#pragma once

#include "mc/server/commands/CommandOutput.h"
namespace carpet_mod_for_ll {


// Prof命令管理类
class ProfCommand {
public:
    // 注册命令到LeviLamina
    static void registerCommand();

private:
    // 命令处理函数
    static void handleNormal(CommandOutput& output);
    static void handleChunk(CommandOutput& output);
    static void handleEntity(CommandOutput& output);
    static void handlePendingTick(CommandOutput& output);
    static void handleMSPT(CommandOutput& output);
    static void handleStop(CommandOutput& output);

    // 获取Profiler模块的辅助函数
    static class ProfilerModule* getProfilerModule();
};

} // namespace carpet_mod_for_ll