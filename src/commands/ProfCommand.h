#pragma once

#include <string>

namespace carpet_mod_for_ll {

// Prof命令参数结构
struct ProfCommandParams {
    enum class Action {
        Normal,
        Chunk,
        Entity,
        PendingTick,
        MSPT,
        Stop
    };
    
    Action action;
};

// Prof命令管理类
class ProfCommand {
public:
    // 注册命令到LeviLamina
    static void registerCommand();

private:
    // 命令处理函数
    static void handleNormal();
    static void handleChunk();
    static void handleEntity();
    static void handlePendingTick();
    static void handleMSPT();
    static void handleStop();
    
    // 获取Profiler模块的辅助函数
    static class ProfilerModule* getProfilerModule();
};

} // namespace carpet_mod_for_ll