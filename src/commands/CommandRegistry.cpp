#include "CommandRegistry.h"
#include <ll/api/mod/NativeMod.h>

#include "CarpetCommand.h"
#include "ProfCommand.h"
#include "TickCommand.h"


namespace carpet_mod_for_ll {

void CommandRegistry::registerAllCommands() {
    auto mod = ll::mod::NativeMod::current();
    mod->getLogger().info("Registering all commands...");

    try {
        // 注册Carpet主命令
        CarpetCommand::registerCommand();

        // 注册Tick控制命令
        TickCommand::registerCommand();

        // 注册性能分析命令
        ProfCommand::registerCommand();

        mod->getLogger().info("All commands registered successfully");

    } catch (const std::exception& e) {
        mod->getLogger().error("Failed to register commands: {}", e.what());
    }
}

void CommandRegistry::cleanup() {
    auto mod = ll::mod::NativeMod::current();
    mod->getLogger().info("Cleaning up commands...");

    try {
        // LeviLamina的DynamicCommand会在CommandRegistrar析构时自动清理
        // 这里主要是为了保持API一致性和日志记录

        mod->getLogger().info("Commands cleanup completed");

    } catch (const std::exception& e) {
        mod->getLogger().error("Failed to cleanup commands: {}", e.what());
    }
}

} // namespace carpet_mod_for_ll