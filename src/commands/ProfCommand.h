#pragma once

#include "BaseCommand.h"
#include "../features/ProfilerModule.h"

namespace carpet_mod_for_ll {

class ProfCommand : public BaseCommand {
public:
    ProfCommand();
    
    bool registerCommand() override;
    
private:
    mutable ProfilerModule* profilerModule = nullptr;
    
    // Helper to get module lazily
    ProfilerModule* getProfilerModule() const;
    
    // 子命令处理器
    void handleNormal(const CommandContext& ctx);
    void handleChunk(const CommandContext& ctx);
    void handleEntity(const CommandContext& ctx);
    void handlePendingTick(const CommandContext& ctx);
    void handleMSPT(const CommandContext& ctx);
    void handleStop(const CommandContext& ctx);
};

} // namespace carpet_mod_for_ll