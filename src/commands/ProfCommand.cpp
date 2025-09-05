#include "ProfCommand.h"
#include "../features/ProfilerModule.h"
#include "../functions/BaseModule.h"
#include <ll/api/mod/NativeMod.h>
#include <ll/api/command/CommandRegistrar.h>
#include <ll/api/command/CommandHandle.h>
#include <mc/server/commands/CommandOrigin.h>
#include <mc/server/commands/CommandOutput.h>
#include <mc/server/commands/CommandPermissionLevel.h>

namespace carpet_mod_for_ll {

void ProfCommand::registerCommand() {
    auto mod = ll::mod::NativeMod::current();
    
    try {
        // 使用LeviLamina标准API注册命令
        auto& command = ll::command::CommandRegistrar::getInstance().getOrCreateCommand(
            "prof",
            "Profiling command",
            CommandPermissionLevel::Admin  // Profiling命令需要管理员权限
        );
        
        // /prof - 默认执行normal
        command.overload()
            .execute([](CommandOrigin const&, CommandOutput& output) {
                ProfCommand::handleNormal();
            });
        
        // /prof normal
        command.overload()
            .text("normal")
            .execute([](CommandOrigin const&, CommandOutput& output) {
                ProfCommand::handleNormal();
            });
        
        // /prof chunk
        command.overload()
            .text("chunk")
            .execute([](CommandOrigin const&, CommandOutput& output) {
                ProfCommand::handleChunk();
            });
        
        // /prof entity
        command.overload()
            .text("entity")
            .execute([](CommandOrigin const&, CommandOutput& output) {
                ProfCommand::handleEntity();
            });
        
        // /prof pt
        command.overload()
            .text("pt")
            .execute([](CommandOrigin const&, CommandOutput& output) {
                ProfCommand::handlePendingTick();
            });
        
        // /prof pendingtick
        command.overload()
            .text("pendingtick")
            .execute([](CommandOrigin const&, CommandOutput& output) {
                ProfCommand::handlePendingTick();
            });
        
        // /prof mspt
        command.overload()
            .text("mspt")
            .execute([](CommandOrigin const&, CommandOutput& output) {
                ProfCommand::handleMSPT();
            });
        
        // /prof stop
        command.overload()
            .text("stop")
            .execute([](CommandOrigin const&, CommandOutput& output) {
                ProfCommand::handleStop();
            });
        
        mod->getLogger().info("Prof command registered successfully");
        
    } catch (const std::exception& e) {
        mod->getLogger().error("Failed to register prof command: {}", e.what());
    }
}

ProfilerModule* ProfCommand::getProfilerModule() {
    static ProfilerModule* profilerModule = nullptr;
    if (!profilerModule) {
        profilerModule = dynamic_cast<ProfilerModule*>(ModuleManager::getModule("Profiler"));
    }
    return profilerModule;
}

void ProfCommand::handleNormal() {
    auto mod = ll::mod::NativeMod::current();
    auto& logger = mod->getLogger();
    
    auto* module = getProfilerModule();
    if (!module) {
        logger.error("§cProfiler module not found");
        return;
    }
    
    if (!module->isEnabled()) {
        logger.error("§cProfiler module is disabled");
        return;
    }
    
    int ticks = 100; // Default value
    module->profileNormal(ticks);
    logger.info("§aStarting normal profiling for {} ticks", ticks);
}

void ProfCommand::handleChunk() {
    auto mod = ll::mod::NativeMod::current();
    auto& logger = mod->getLogger();
    
    auto* module = getProfilerModule();
    if (!module) {
        logger.error("§cProfiler module not found");
        return;
    }
    
    if (!module->isEnabled()) {
        logger.error("§cProfiler module is disabled");
        return;
    }
    
    int ticks = 100; // Default value
    module->profileChunk(ticks);
    logger.info("§aStarting chunk profiling for {} ticks", ticks);
}

void ProfCommand::handleEntity() {
    auto mod = ll::mod::NativeMod::current();
    auto& logger = mod->getLogger();
    
    auto* module = getProfilerModule();
    if (!module) {
        logger.error("§cProfiler module not found");
        return;
    }
    
    if (!module->isEnabled()) {
        logger.error("§cProfiler module is disabled");
        return;
    }
    
    int ticks = 100; // Default value
    module->profileEntity(ticks);
    logger.info("§aStarting entity profiling for {} ticks", ticks);
}

void ProfCommand::handlePendingTick() {
    auto mod = ll::mod::NativeMod::current();
    auto& logger = mod->getLogger();
    
    auto* module = getProfilerModule();
    if (!module) {
        logger.error("§cProfiler module not found");
        return;
    }
    
    if (!module->isEnabled()) {
        logger.error("§cProfiler module is disabled");
        return;
    }
    
    int ticks = 100; // Default value
    module->profilePendingTick(ticks);
    logger.info("§aStarting pending tick profiling for {} ticks", ticks);
}

void ProfCommand::handleMSPT() {
    auto mod = ll::mod::NativeMod::current();
    auto& logger = mod->getLogger();
    
    auto* module = getProfilerModule();
    if (!module) {
        logger.error("§cProfiler module not found");
        return;
    }
    
    if (!module->isEnabled()) {
        logger.error("§cProfiler module is disabled");
        return;
    }
    
    int ticks = 100; // Default value
    module->profileMSPT(ticks);
    logger.info("§aStarting MSPT profiling for {} ticks", ticks);
}

void ProfCommand::handleStop() {
    auto mod = ll::mod::NativeMod::current();
    auto& logger = mod->getLogger();
    
    auto* module = getProfilerModule();
    if (!module) {
        logger.error("§cProfiler module not found");
        return;
    }
    
    if (!module->isEnabled()) {
        logger.error("§cProfiler module is disabled");
        return;
    }
    
    module->stopProfiling();
    logger.info("§aProfiling stopped");
}

} // namespace carpet_mod_for_ll