#include "TickCommand.h"
#include "../features/TickModule.h"
#include "../functions/BaseModule.h"
#include "../utils/ConfigManager.h"
#include <ll/api/command/CommandHandle.h>
#include <ll/api/command/CommandRegistrar.h>
#include <ll/api/mod/NativeMod.h>
#include <mc/server/commands/CommandOrigin.h>
#include <mc/server/commands/CommandOutput.h>
#include <mc/server/commands/CommandPermissionLevel.h>


namespace carpet_mod_for_ll {

// Tick命令参数结构体
struct TickIntParam {
    int value;
};

} // namespace carpet_mod_for_ll

// 使用boost::pfr注册反射信息（LeviLamina使用boost::pfr进行反射）
// 这个结构体会自动被boost::pfr识别，无需额外注册

namespace carpet_mod_for_ll {

void TickCommand::registerCommand() {
    auto mod = ll::mod::NativeMod::current();

    try {
        // 使用LeviLamina标准API注册命令
        auto& command = ll::command::CommandRegistrar::getInstance().getOrCreateCommand(
            "tick",
            "Tick control command",
            ConfigManager::getCommandPermissionLevel(ConfigManager::getInstance().getConfig().commands.permissionLevel)
        );

        // /tick freeze, /tick fz
        command.overload().text("freeze").execute([](CommandOrigin const&, CommandOutput& output) {
            TickCommand::handleFreeze(output);
        });

        command.overload().text("fz").execute([](CommandOrigin const&, CommandOutput& output) {
            TickCommand::handleFreeze(output);
        });

        // /tick reset, /tick r
        command.overload().text("reset").execute([](CommandOrigin const&, CommandOutput& output) {
            TickCommand::handleReset(output);
        });

        command.overload().text("r").execute([](CommandOrigin const&, CommandOutput& output) {
            TickCommand::handleReset(output);
        });

        // /tick forward <ticks>, /tick fw <ticks>
        command.overload<TickIntParam>().text("forward").required("value").execute(
            [](CommandOrigin const&, CommandOutput& output, TickIntParam const& params) {
                TickCommand::handleForward(output, params.value);
            }
        );

        command.overload<TickIntParam>().text("fw").required("value").execute(
            [](CommandOrigin const&, CommandOutput& output, TickIntParam const& params) {
                TickCommand::handleForward(output, params.value);
            }
        );

        // /tick warp <ticks>
        command.overload<TickIntParam>().text("warp").required("value").execute(
            [](CommandOrigin const&, CommandOutput& output, TickIntParam const& params) {
                TickCommand::handleWarp(output, params.value);
            }
        );

        // /tick acc <multiplier>
        command.overload<TickIntParam>().text("acc").required("value").execute(
            [](CommandOrigin const&, CommandOutput& output, TickIntParam const& params) {
                TickCommand::handleAcc(output, params.value);
            }
        );

        // /tick slow <divider>
        command.overload<TickIntParam>().text("slow").required("value").execute(
            [](CommandOrigin const&, CommandOutput& output, TickIntParam const& params) {
                TickCommand::handleSlow(output, params.value);
            }
        );

        // /tick query
        command.overload().text("query").execute([](CommandOrigin const&, CommandOutput& output) {
            TickCommand::handleQuery(output);
        });

        // /tick mspt
        command.overload().text("mspt").execute([](CommandOrigin const&, CommandOutput& output) {
            TickCommand::handleMSPT(output);
        });

        mod->getLogger().info("Tick command registered successfully");

    } catch (const std::exception& e) {
        mod->getLogger().error("Failed to register tick command: {}", e.what());
    }
}

TickModule* TickCommand::getTickModule() {
    static TickModule* tickModule = nullptr;
    if (!tickModule) {
        tickModule = dynamic_cast<TickModule*>(ModuleManager::getModule("TickControl"));
    }
    return tickModule;
}

void TickCommand::handleFreeze(CommandOutput& output) {


    auto* module = getTickModule();
    if (!module) {
        output.error("§cTickControl module not found");
        return;
    }

    if (!module->isEnabled()) {
        output.error("§cTickControl module is disabled");
        return;
    }

    module->freezeTick();
    output.success("§aTick has been frozen");
}

void TickCommand::handleReset(CommandOutput& output) {

    auto* module = getTickModule();
    if (!module) {
        output.error("§cTickControl module not found");
        return;
    }

    if (!module->isEnabled()) {
        output.error("§cTickControl module is disabled");
        return;
    }

    module->resetTick();
    output.success("§aTick has been reset to normal");
}

void TickCommand::handleForward(CommandOutput& output, int ticks) {

    if (ticks <= 0) {
        output.error("§cTicks must be a positive number");
        return;
    }

    auto* module = getTickModule();
    if (!module) {
        output.error("§cTickControl module not found");
        return;
    }

    if (!module->isEnabled()) {
        output.error("§cTickControl module is disabled");
        return;
    }

    module->forwardTick(ticks);
    output.success("§aForwarded {} ticks", ticks);
}

void TickCommand::handleWarp(CommandOutput& output, int ticks) {

    if (ticks <= 0) {
        output.error("§cTicks must be a positive number");
        return;
    }

    auto* module = getTickModule();
    if (!module) {
        output.error("§cTickControl module not found");
        return;
    }

    if (!module->isEnabled()) {
        output.error("§cTickControl module is disabled");
        return;
    }

    module->warpTick(ticks);
    output.success("§aWarping {} ticks...", ticks);
}

void TickCommand::handleAcc(CommandOutput& output, int multiplier) {

    if (multiplier <= 0) {
        output.error("§cMultiplier must be a positive number");
        return;
    }

    auto* module = getTickModule();
    if (!module) {
        output.error("§cTickControl module not found");
        return;
    }

    if (!module->isEnabled()) {
        output.error("§cTickControl module is disabled");
        return;
    }

    module->accelerateTick(multiplier);
    output.success("§aTick acceleration set to {}x", multiplier);
}

void TickCommand::handleSlow(CommandOutput& output, int divider) {

    if (divider <= 0) {
        output.error("§cDivider must be a positive number");
        return;
    }

    auto* module = getTickModule();
    if (!module) {
        output.error("§cTickControl module not found");
        return;
    }

    if (!module->isEnabled()) {
        output.error("§cTickControl module is disabled");
        return;
    }

    module->slowDownTick(divider);
    output.success("§aTick slowed down by factor of {}", divider);
}

void TickCommand::handleQuery(CommandOutput& output) {

    auto* module = getTickModule();
    if (!module) {
        output.error("§cTickControl module not found");
        return;
    }

    if (!module->isEnabled()) {
        output.error("§cTickControl module is disabled");
        return;
    }

    module->queryTickStatus();
    // Status will be logged by the module itself
}

void TickCommand::handleMSPT(CommandOutput& output) {

    auto* module = getTickModule();
    if (!module) {
        output.error("§cTickControl module not found");
        return;
    }

    if (!module->isEnabled()) {
        output.error("§cTickControl module is disabled");
        return;
    }

    module->showMSPT();
    // MSPT info will be logged by the module itself
}

} // namespace carpet_mod_for_ll