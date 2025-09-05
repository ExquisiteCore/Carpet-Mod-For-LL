#include "TickCommand.h"
#include "../features/TickModule.h"
#include "../functions/BaseModule.h"
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
            CommandPermissionLevel::Any // Tick命令需要管理员权限
        );

        // /tick freeze, /tick fz
        command.overload().text("freeze").execute([](CommandOrigin const&, CommandOutput& output) {
            TickCommand::handleFreeze();
        });

        command.overload().text("fz").execute([](CommandOrigin const&, CommandOutput& output) {
            TickCommand::handleFreeze();
        });

        // /tick reset, /tick r
        command.overload().text("reset").execute([](CommandOrigin const&, CommandOutput& output) {
            TickCommand::handleReset();
        });

        command.overload().text("r").execute([](CommandOrigin const&, CommandOutput& output) {
            TickCommand::handleReset();
        });

        // /tick forward <ticks>, /tick fw <ticks>
        command.overload<TickIntParam>().text("forward").required("value").execute(
            [](CommandOrigin const&, CommandOutput& output, TickIntParam const& params) {
                TickCommand::handleForward(params.value);
            }
        );

        command.overload<TickIntParam>().text("fw").required("value").execute(
            [](CommandOrigin const&, CommandOutput& output, TickIntParam const& params) {
                TickCommand::handleForward(params.value);
            }
        );

        // /tick warp <ticks>
        command.overload<TickIntParam>().text("warp").required("value").execute(
            [](CommandOrigin const&, CommandOutput& output, TickIntParam const& params) {
                TickCommand::handleWarp(params.value);
            }
        );

        // /tick acc <multiplier>
        command.overload<TickIntParam>().text("acc").required("value").execute(
            [](CommandOrigin const&, CommandOutput& output, TickIntParam const& params) {
                TickCommand::handleAcc(params.value);
            }
        );

        // /tick slow <divider>
        command.overload<TickIntParam>().text("slow").required("value").execute(
            [](CommandOrigin const&, CommandOutput& output, TickIntParam const& params) {
                TickCommand::handleSlow(params.value);
            }
        );

        // /tick query
        command.overload().text("query").execute([](CommandOrigin const&, CommandOutput& output) {
            TickCommand::handleQuery();
        });

        // /tick mspt
        command.overload().text("mspt").execute([](CommandOrigin const&, CommandOutput& output) {
            TickCommand::handleMSPT();
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

void TickCommand::handleFreeze() {
    auto  mod    = ll::mod::NativeMod::current();
    auto& logger = mod->getLogger();

    auto* module = getTickModule();
    if (!module) {
        logger.error("§cTickControl module not found");
        return;
    }

    if (!module->isEnabled()) {
        logger.error("§cTickControl module is disabled");
        return;
    }

    module->freezeTick();
    logger.info("§aTick has been frozen");
}

void TickCommand::handleReset() {
    auto  mod    = ll::mod::NativeMod::current();
    auto& logger = mod->getLogger();

    auto* module = getTickModule();
    if (!module) {
        logger.error("§cTickControl module not found");
        return;
    }

    if (!module->isEnabled()) {
        logger.error("§cTickControl module is disabled");
        return;
    }

    module->resetTick();
    logger.info("§aTick has been reset to normal");
}

void TickCommand::handleForward(int ticks) {
    auto  mod    = ll::mod::NativeMod::current();
    auto& logger = mod->getLogger();

    if (ticks <= 0) {
        logger.error("§cTicks must be a positive number");
        return;
    }

    auto* module = getTickModule();
    if (!module) {
        logger.error("§cTickControl module not found");
        return;
    }

    if (!module->isEnabled()) {
        logger.error("§cTickControl module is disabled");
        return;
    }

    module->forwardTick(ticks);
    logger.info("§aForwarded {} ticks", ticks);
}

void TickCommand::handleWarp(int ticks) {
    auto  mod    = ll::mod::NativeMod::current();
    auto& logger = mod->getLogger();

    if (ticks <= 0) {
        logger.error("§cTicks must be a positive number");
        return;
    }

    auto* module = getTickModule();
    if (!module) {
        logger.error("§cTickControl module not found");
        return;
    }

    if (!module->isEnabled()) {
        logger.error("§cTickControl module is disabled");
        return;
    }

    module->warpTick(ticks);
    logger.info("§aWarping {} ticks...", ticks);
}

void TickCommand::handleAcc(int multiplier) {
    auto  mod    = ll::mod::NativeMod::current();
    auto& logger = mod->getLogger();

    if (multiplier <= 0) {
        logger.error("§cMultiplier must be a positive number");
        return;
    }

    auto* module = getTickModule();
    if (!module) {
        logger.error("§cTickControl module not found");
        return;
    }

    if (!module->isEnabled()) {
        logger.error("§cTickControl module is disabled");
        return;
    }

    module->accelerateTick(multiplier);
    logger.info("§aTick acceleration set to {}x", multiplier);
}

void TickCommand::handleSlow(int divider) {
    auto  mod    = ll::mod::NativeMod::current();
    auto& logger = mod->getLogger();

    if (divider <= 0) {
        logger.error("§cDivider must be a positive number");
        return;
    }

    auto* module = getTickModule();
    if (!module) {
        logger.error("§cTickControl module not found");
        return;
    }

    if (!module->isEnabled()) {
        logger.error("§cTickControl module is disabled");
        return;
    }

    module->slowDownTick(divider);
    logger.info("§aTick slowed down by factor of {}", divider);
}

void TickCommand::handleQuery() {
    auto  mod    = ll::mod::NativeMod::current();
    auto& logger = mod->getLogger();

    auto* module = getTickModule();
    if (!module) {
        logger.error("§cTickControl module not found");
        return;
    }

    if (!module->isEnabled()) {
        logger.error("§cTickControl module is disabled");
        return;
    }

    module->queryTickStatus();
    // Status will be logged by the module itself
}

void TickCommand::handleMSPT() {
    auto  mod    = ll::mod::NativeMod::current();
    auto& logger = mod->getLogger();

    auto* module = getTickModule();
    if (!module) {
        logger.error("§cTickControl module not found");
        return;
    }

    if (!module->isEnabled()) {
        logger.error("§cTickControl module is disabled");
        return;
    }

    module->showMSPT();
    // MSPT info will be logged by the module itself
}

} // namespace carpet_mod_for_ll