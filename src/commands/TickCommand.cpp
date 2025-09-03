#include "TickCommand.h"
#include "../functions/BaseModule.h"
#include <ll/api/i18n/I18n.h>
#include <ll/api/mod/NativeMod.h>

namespace carpet_mod_for_ll {

using ll::i18n_literals::operator""_tr;

TickCommand::TickCommand() 
    : BaseCommand("tick", "carpet.command.tick.description"_tr(), CommandPermission::Admin) {
}

bool TickCommand::registerCommand() {
    // Don't get module here, get it lazily in handlers
    
    // /tick freeze, /tick fz
    addSubCommand({
        .name = "freeze",
        .description = "carpet.command.tick.freeze.description"_tr(),
        .usage = "/tick freeze",
        .handler = [this](const CommandContext& ctx) { handleFreeze(ctx); },
        .aliases = {"fz"}
    });
    
    // /tick reset, /tick r
    addSubCommand({
        .name = "reset",
        .description = "carpet.command.tick.reset.description"_tr(),
        .usage = "/tick reset",
        .handler = [this](const CommandContext& ctx) { handleReset(ctx); },
        .aliases = {"r"}
    });
    
    // /tick forward <ticks>, /tick fw <ticks>
    addSubCommand({
        .name = "forward",
        .description = "carpet.command.tick.forward.description"_tr(),
        .usage = "/tick forward <ticks>",
        .handler = [this](const CommandContext& ctx) { handleForward(ctx); },
        .aliases = {"fw"}
    });
    
    // /tick warp <ticks>
    addSubCommand({
        .name = "warp",
        .description = "carpet.command.tick.warp.description"_tr(),
        .usage = "/tick warp <ticks>",
        .handler = [this](const CommandContext& ctx) { handleWarp(ctx); }
    });
    
    // /tick acc <multiplier>
    addSubCommand({
        .name = "acc",
        .description = "carpet.command.tick.acc.description"_tr(),
        .usage = "/tick acc <multiplier>",
        .handler = [this](const CommandContext& ctx) { handleAcc(ctx); }
    });
    
    // /tick slow <divider>
    addSubCommand({
        .name = "slow",
        .description = "carpet.command.tick.slow.description"_tr(),
        .usage = "/tick slow <divider>",
        .handler = [this](const CommandContext& ctx) { handleSlow(ctx); }
    });
    
    // /tick query
    addSubCommand({
        .name = "query",
        .description = "carpet.command.tick.query.description"_tr(),
        .usage = "/tick query",
        .handler = [this](const CommandContext& ctx) { handleQuery(ctx); }
    });
    
    // /tick mspt
    addSubCommand({
        .name = "mspt",
        .description = "carpet.command.tick.mspt.description"_tr(),
        .usage = "/tick mspt",
        .handler = [this](const CommandContext& ctx) { handleMSPT(ctx); }
    });
    
    // TODO: Register with LeviLamina command system
    auto mod = ll::mod::NativeMod::current();
    mod->getLogger().info("TickCommand registered");
    
    return BaseCommand::registerCommand();
}

TickModule* TickCommand::getTickModule() const {
    if (!tickModule) {
        tickModule = dynamic_cast<TickModule*>(ModuleManager::getModule("TickControl"));
    }
    return tickModule;
}

void TickCommand::handleFreeze(const CommandContext& ctx) {
    auto* module = getTickModule();
    if (!module) {
        ctx.error("Module TickControl not found");
        return;
    }
    
    if (!module->isEnabled()) {
        ctx.error("carpet.error.module.disabled"_tr("TickControl"));
        return;
    }
    
    module->freezeTick();
    ctx.success("carpet.command.tick.freeze.success"_tr());
}

void TickCommand::handleReset(const CommandContext& ctx) {
    auto* module = getTickModule();
    if (!module) {
        ctx.error("Module TickControl not found");
        return;
    }
    
    if (!module->isEnabled()) {
        ctx.error("carpet.error.module.disabled"_tr("TickControl"));
        return;
    }
    
    module->resetTick();
    ctx.success("carpet.command.tick.reset.success"_tr());
}

void TickCommand::handleForward(const CommandContext& ctx) {
    auto* module = getTickModule();
    if (!module) {
        ctx.error("Module TickControl not found");
        return;
    }
    
    if (!module->isEnabled()) {
        ctx.error("carpet.error.module.disabled"_tr("TickControl"));
        return;
    }
    
    if (ctx.args.size() < 2) {
        ctx.error("carpet.command.tick.forward.usage"_tr());
        return;
    }
    
    try {
        int ticks = std::stoi(ctx.args[1]);
        module->forwardTick(ticks);
        ctx.success("carpet.command.tick.forward.success"_tr(ticks));
    } catch (const std::exception& e) {
        ctx.error("carpet.command.tick.invalid.number"_tr());
    }
}

void TickCommand::handleWarp(const CommandContext& ctx) {
    auto* module = getTickModule();
    if (!module) {
        ctx.error("Module TickControl not found");
        return;
    }
    
    if (!module->isEnabled()) {
        ctx.error("carpet.error.module.disabled"_tr("TickControl"));
        return;
    }
    
    if (ctx.args.size() < 2) {
        ctx.error("carpet.command.tick.warp.usage"_tr());
        return;
    }
    
    try {
        int ticks = std::stoi(ctx.args[1]);
        module->warpTick(ticks);
        ctx.success("carpet.command.tick.warp.success"_tr(ticks));
    } catch (const std::exception& e) {
        ctx.error("carpet.command.tick.invalid.number"_tr());
    }
}

void TickCommand::handleAcc(const CommandContext& ctx) {
    auto* module = getTickModule();
    if (!module) {
        ctx.error("Module TickControl not found");
        return;
    }
    
    if (!module->isEnabled()) {
        ctx.error("carpet.error.module.disabled"_tr("TickControl"));
        return;
    }
    
    if (ctx.args.size() < 2) {
        ctx.error("carpet.command.tick.acc.usage"_tr());
        return;
    }
    
    try {
        int multiplier = std::stoi(ctx.args[1]);
        module->accelerateTick(multiplier);
        ctx.success("carpet.command.tick.acc.success"_tr(multiplier));
    } catch (const std::exception& e) {
        ctx.error("carpet.command.tick.invalid.number"_tr());
    }
}

void TickCommand::handleSlow(const CommandContext& ctx) {
    auto* module = getTickModule();
    if (!module) {
        ctx.error("Module TickControl not found");
        return;
    }
    
    if (!module->isEnabled()) {
        ctx.error("carpet.error.module.disabled"_tr("TickControl"));
        return;
    }
    
    if (ctx.args.size() < 2) {
        ctx.error("carpet.command.tick.slow.usage"_tr());
        return;
    }
    
    try {
        int divider = std::stoi(ctx.args[1]);
        module->slowDownTick(divider);
        ctx.success("carpet.command.tick.slow.success"_tr(divider));
    } catch (const std::exception& e) {
        ctx.error("carpet.command.tick.invalid.number"_tr());
    }
}

void TickCommand::handleQuery(const CommandContext& ctx) {
    auto* module = getTickModule();
    if (!module) {
        ctx.error("Module TickControl not found");
        return;
    }
    
    if (!module->isEnabled()) {
        ctx.error("carpet.error.module.disabled"_tr("TickControl"));
        return;
    }
    
    module->queryTickStatus();
    // Status will be printed by the module itself
}

void TickCommand::handleMSPT(const CommandContext& ctx) {
    auto* module = getTickModule();
    if (!module) {
        ctx.error("Module TickControl not found");
        return;
    }
    
    if (!module->isEnabled()) {
        ctx.error("carpet.error.module.disabled"_tr("TickControl"));
        return;
    }
    
    module->showMSPT();
    // MSPT info will be printed by the module itself
}

} // namespace carpet_mod_for_ll