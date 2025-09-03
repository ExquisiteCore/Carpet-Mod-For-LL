#pragma once

#include "BaseCommand.h"
#include "../features/TickModule.h"

namespace carpet_mod_for_ll {

class TickCommand : public BaseCommand {
public:
    TickCommand();
    
    bool registerCommand() override;
    
private:
    TickModule* tickModule = nullptr;
    
    // 子命令处理器
    void handleFreeze(const CommandContext& ctx);
    void handleReset(const CommandContext& ctx);
    void handleForward(const CommandContext& ctx);
    void handleWarp(const CommandContext& ctx);
    void handleAcc(const CommandContext& ctx);
    void handleSlow(const CommandContext& ctx);
    void handleQuery(const CommandContext& ctx);
    void handleMSPT(const CommandContext& ctx);
};

} // namespace carpet_mod_for_ll