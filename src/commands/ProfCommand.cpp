#include "ProfCommand.h"
#include "../functions/BaseModule.h"
#include <ll/api/i18n/I18n.h>
#include <ll/api/mod/NativeMod.h>

namespace carpet_mod_for_ll {

using ll::i18n_literals::operator""_tr;

ProfCommand::ProfCommand() 
    : BaseCommand("prof", "carpet.command.prof.description"_tr(), CommandPermission::Admin) {
}

bool ProfCommand::registerCommand() {
    // Get profiler module
    profilerModule = dynamic_cast<ProfilerModule*>(ModuleManager::getModule("Profiler"));
    if (!profilerModule) {
        auto mod = ll::mod::NativeMod::current();
        mod->getLogger().error("ProfilerModule not found!");
        return false;
    }
    
    // /prof or /prof normal [ticks]
    addSubCommand({
        .name = "normal",
        .description = "carpet.command.prof.normal.description"_tr(),
        .usage = "/prof normal [ticks]",
        .handler = [this](const CommandContext& ctx) { handleNormal(ctx); }
    });
    
    // /prof chunk [ticks]
    addSubCommand({
        .name = "chunk",
        .description = "carpet.command.prof.chunk.description"_tr(),
        .usage = "/prof chunk [ticks]",
        .handler = [this](const CommandContext& ctx) { handleChunk(ctx); }
    });
    
    // /prof entity [ticks]
    addSubCommand({
        .name = "entity",
        .description = "carpet.command.prof.entity.description"_tr(),
        .usage = "/prof entity [ticks]",
        .handler = [this](const CommandContext& ctx) { handleEntity(ctx); }
    });
    
    // /prof pt [ticks] or /prof pendingtick [ticks]
    addSubCommand({
        .name = "pt",
        .description = "carpet.command.prof.pt.description"_tr(),
        .usage = "/prof pt [ticks]",
        .handler = [this](const CommandContext& ctx) { handlePendingTick(ctx); },
        .aliases = {"pendingtick"}
    });
    
    // /prof mspt [ticks]
    addSubCommand({
        .name = "mspt",
        .description = "carpet.command.prof.mspt.description"_tr(),
        .usage = "/prof mspt [ticks]",
        .handler = [this](const CommandContext& ctx) { handleMSPT(ctx); }
    });
    
    // /prof stop
    addSubCommand({
        .name = "stop",
        .description = "carpet.command.prof.stop.description"_tr(),
        .usage = "/prof stop",
        .handler = [this](const CommandContext& ctx) { handleStop(ctx); }
    });
    
    // TODO: Register with LeviLamina command system
    auto mod = ll::mod::NativeMod::current();
    mod->getLogger().info("ProfCommand registered");
    
    return BaseCommand::registerCommand();
}

void ProfCommand::handleNormal(const CommandContext& ctx) {
    if (!profilerModule->isEnabled()) {
        ctx.error("carpet.error.module.disabled"_tr("Profiler"));
        return;
    }
    
    int ticks = 100; // Default
    if (ctx.args.size() >= 2) {
        try {
            ticks = std::stoi(ctx.args[1]);
        } catch (const std::exception& e) {
            ctx.error("carpet.command.prof.invalid.number"_tr());
            return;
        }
    }
    
    profilerModule->profileNormal(ticks);
    ctx.success("carpet.command.prof.normal.start"_tr(ticks));
}

void ProfCommand::handleChunk(const CommandContext& ctx) {
    if (!profilerModule->isEnabled()) {
        ctx.error("carpet.error.module.disabled"_tr("Profiler"));
        return;
    }
    
    int ticks = 100; // Default
    if (ctx.args.size() >= 2) {
        try {
            ticks = std::stoi(ctx.args[1]);
        } catch (const std::exception& e) {
            ctx.error("carpet.command.prof.invalid.number"_tr());
            return;
        }
    }
    
    profilerModule->profileChunk(ticks);
    ctx.success("carpet.command.prof.chunk.start"_tr(ticks));
}

void ProfCommand::handleEntity(const CommandContext& ctx) {
    if (!profilerModule->isEnabled()) {
        ctx.error("carpet.error.module.disabled"_tr("Profiler"));
        return;
    }
    
    int ticks = 100; // Default
    if (ctx.args.size() >= 2) {
        try {
            ticks = std::stoi(ctx.args[1]);
        } catch (const std::exception& e) {
            ctx.error("carpet.command.prof.invalid.number"_tr());
            return;
        }
    }
    
    profilerModule->profileEntity(ticks);
    ctx.success("carpet.command.prof.entity.start"_tr(ticks));
}

void ProfCommand::handlePendingTick(const CommandContext& ctx) {
    if (!profilerModule->isEnabled()) {
        ctx.error("carpet.error.module.disabled"_tr("Profiler"));
        return;
    }
    
    int ticks = 100; // Default
    if (ctx.args.size() >= 2) {
        try {
            ticks = std::stoi(ctx.args[1]);
        } catch (const std::exception& e) {
            ctx.error("carpet.command.prof.invalid.number"_tr());
            return;
        }
    }
    
    profilerModule->profilePendingTick(ticks);
    ctx.success("carpet.command.prof.pt.start"_tr(ticks));
}

void ProfCommand::handleMSPT(const CommandContext& ctx) {
    if (!profilerModule->isEnabled()) {
        ctx.error("carpet.error.module.disabled"_tr("Profiler"));
        return;
    }
    
    int ticks = 100; // Default
    if (ctx.args.size() >= 2) {
        try {
            ticks = std::stoi(ctx.args[1]);
        } catch (const std::exception& e) {
            ctx.error("carpet.command.prof.invalid.number"_tr());
            return;
        }
    }
    
    profilerModule->profileMSPT(ticks);
    ctx.success("carpet.command.prof.mspt.start"_tr(ticks));
}

void ProfCommand::handleStop(const CommandContext& ctx) {
    if (!profilerModule->isEnabled()) {
        ctx.error("carpet.error.module.disabled"_tr("Profiler"));
        return;
    }
    
    profilerModule->stopProfiling();
    ctx.success("carpet.command.prof.stop.success"_tr());
}

} // namespace carpet_mod_for_ll