#include "ProfCommand.h"
#include "../features/ProfilerModule.h"
#include "../functions/BaseModule.h"
#include "../utils/ConfigManager.h"

#include <ll/api/command/CommandHandle.h>
#include <ll/api/command/CommandRegistrar.h>
#include <ll/api/mod/NativeMod.h>
#include <mc/server/commands/CommandOrigin.h>
#include <mc/server/commands/CommandOutput.h>
#include <mc/server/commands/CommandPermissionLevel.h>


namespace carpet_mod_for_ll {

// Prof命令参数结构体
struct ProfTicksParam {
    int ticks;
};

} // namespace carpet_mod_for_ll

namespace carpet_mod_for_ll {

void ProfCommand::registerCommand() {
    auto mod = ll::mod::NativeMod::current();

    try {
        // 使用LeviLamina标准API注册命令
        auto& command = ll::command::CommandRegistrar::getInstance().getOrCreateCommand(
            "prof",
            "Profiling command",
            ConfigManager::getCommandPermissionLevel(ConfigManager::getInstance().getConfig().commands.permissionLevel)
        );

        // /prof - 默认执行normal
        command.overload().execute([](CommandOrigin const&, CommandOutput& output) {
            ProfCommand::handleNormal(output);
        });

        // /prof normal [ticks]
        command.overload().text("normal").execute([](CommandOrigin const&, CommandOutput& output) {
            ProfCommand::handleNormal(output);
        });

        command.overload<ProfTicksParam>().text("normal").required("ticks").execute(
            [](CommandOrigin const&, CommandOutput& output, ProfTicksParam const& params) {
                // TODO: Pass ticks parameter to handler
                ProfCommand::handleNormal(output);
            }
        );

        // /prof chunk [ticks]
        command.overload().text("chunk").execute([](CommandOrigin const&, CommandOutput& output) {
            ProfCommand::handleChunk(output);
        });

        command.overload<ProfTicksParam>().text("chunk").required("ticks").execute(
            [](CommandOrigin const&, CommandOutput& output, ProfTicksParam const& params) {
                ProfCommand::handleChunk(output);
            }
        );

        // /prof entity [ticks]
        command.overload().text("entity").execute([](CommandOrigin const&, CommandOutput& output) {
            ProfCommand::handleEntity(output);
        });

        command.overload<ProfTicksParam>().text("entity").required("ticks").execute(
            [](CommandOrigin const&, CommandOutput& output, ProfTicksParam const& params) {
                ProfCommand::handleEntity(output);
            }
        );

        // /prof pt [ticks]
        command.overload().text("pt").execute([](CommandOrigin const&, CommandOutput& output) {
            ProfCommand::handlePendingTick(output);
        });

        command.overload<ProfTicksParam>().text("pt").required("ticks").execute(
            [](CommandOrigin const&, CommandOutput& output, ProfTicksParam const& params) {
                ProfCommand::handlePendingTick(output);
            }
        );

        // /prof pendingtick [ticks]
        command.overload().text("pendingtick").execute([](CommandOrigin const&, CommandOutput& output) {
            ProfCommand::handlePendingTick(output);
        });

        command.overload<ProfTicksParam>()
            .text("pendingtick")
            .required("ticks")
            .execute([](CommandOrigin const&, CommandOutput& output, ProfTicksParam const& params) {
                ProfCommand::handlePendingTick(output);
            });

        // /prof mspt [ticks]
        command.overload().text("mspt").execute([](CommandOrigin const&, CommandOutput& output) {
            ProfCommand::handleMSPT(output);
        });

        command.overload<ProfTicksParam>().text("mspt").required("ticks").execute(
            [](CommandOrigin const&, CommandOutput& output, ProfTicksParam const& params) {
                ProfCommand::handleMSPT(output);
            }
        );

        // /prof stop
        command.overload().text("stop").execute([](CommandOrigin const&, CommandOutput& output) {
            ProfCommand::handleStop(output);
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

void ProfCommand::handleNormal(CommandOutput& output) {

    auto* module = getProfilerModule();
    if (!module) {
        output.error("§cProfiler module not found");
        return;
    }

    if (!module->isEnabled()) {
        output.error("§cProfiler module is disabled");
        return;
    }

    int ticks = 100; // Default value
    module->profileNormal(ticks);
    output.success("§aStarting normal profiling for {} ticks", ticks);
}

void ProfCommand::handleChunk(CommandOutput& output) {

    auto* module = getProfilerModule();
    if (!module) {
        output.error("§cProfiler module not found");
        return;
    }

    if (!module->isEnabled()) {
        output.error("§cProfiler module is disabled");
        return;
    }

    int ticks = 100; // Default value
    module->profileChunk(ticks);
    output.success("§aStarting chunk profiling for {} ticks", ticks);
}

void ProfCommand::handleEntity(CommandOutput& output) {

    auto* module = getProfilerModule();
    if (!module) {
        output.error("§cProfiler module not found");
        return;
    }

    if (!module->isEnabled()) {
        output.error("§cProfiler module is disabled");
        return;
    }

    int ticks = 100; // Default value
    module->profileEntity(ticks);
    output.success("§aStarting entity profiling for {} ticks", ticks);
}

void ProfCommand::handlePendingTick(CommandOutput& output) {

    auto* module = getProfilerModule();
    if (!module) {
        output.error("§cProfiler module not found");
        return;
    }

    if (!module->isEnabled()) {
        output.error("§cProfiler module is disabled");
        return;
    }

    int ticks = 100; // Default value
    module->profilePendingTick(ticks);
    output.success("§aStarting pending tick profiling for {} ticks", ticks);
}

void ProfCommand::handleMSPT(CommandOutput& output) {

    auto* module = getProfilerModule();
    if (!module) {
        output.error("§cProfiler module not found");
        return;
    }

    if (!module->isEnabled()) {
        output.error("§cProfiler module is disabled");
        return;
    }

    int ticks = 100; // Default value
    module->profileMSPT(ticks);
    output.success("§aStarting MSPT profiling for {} ticks", ticks);
}

void ProfCommand::handleStop(CommandOutput& output) {

    auto* module = getProfilerModule();
    if (!module) {
        output.error("§cProfiler module not found");
        return;
    }

    if (!module->isEnabled()) {
        output.error("§cProfiler module is disabled");
        return;
    }

    module->stopProfiling();
    output.success("§aProfiling stopped");
}

} // namespace carpet_mod_for_ll