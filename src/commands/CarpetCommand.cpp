#include "CarpetCommand.h"
#include "../functions/BaseModule.h"
#include "../utils/ConfigManager.h"
#include "../utils/I18nManager.h"
#include <ll/api/command/CommandHandle.h>
#include <ll/api/command/CommandRegistrar.h>
#include <ll/api/mod/NativeMod.h>
#include <mc/server/commands/CommandOrigin.h>
#include <mc/server/commands/CommandOutput.h>
#include <mc/server/commands/CommandPermissionLevel.h>


namespace carpet_mod_for_ll {

// Carpet命令参数结构体
struct CarpetFeatureParam {
    std::string feature;
};

struct CarpetConfigParam {
    std::string feature;
    std::string value;
};

} // namespace carpet_mod_for_ll

// boost::pfr会自动识别这些结构体

namespace carpet_mod_for_ll {

void CarpetCommand::registerCommand() {
    auto mod = ll::mod::NativeMod::current();

    try {
        // 使用LeviLamina标准API注册命令
        auto& command = ll::command::CommandRegistrar::getInstance().getOrCreateCommand(
            "carpet",
            "Carpet mod main command",
            ConfigManager::getCommandPermissionLevel(
                ConfigManager::getInstance().getConfig().commands.permissionLevel
            ) // 使用配置中的权限等级
        );

        // 无参数版本 - 显示帮助
        command.overload().execute([](CommandOrigin const&, CommandOutput& output) { CarpetCommand::handleHelp(); });

        // help子命令
        command.overload().text("help").execute([](CommandOrigin const&, CommandOutput& output) {
            CarpetCommand::handleHelp();
        });

        // info子命令
        command.overload().text("info").execute([](CommandOrigin const&, CommandOutput& output) {
            CarpetCommand::handleInfo();
        });

        // list子命令
        command.overload().text("list").execute([](CommandOrigin const&, CommandOutput& output) {
            CarpetCommand::handleList();
        });

        // reload子命令 - 需要管理员权限
        command.overload().text("reload").execute([](CommandOrigin const& origin, CommandOutput& output) {
            if (origin.getPermissionsLevel() < CommandPermissionLevel::Admin) {
                output.error("You don't have permission to reload config");
                return;
            }
            CarpetCommand::handleReload();
        });

        // config子命令 - 无参数版本
        command.overload().text("config").execute([](CommandOrigin const&, CommandOutput& output) {
            CarpetCommand::handleConfig("");
        });

        // config子命令 - 带feature参数
        command.overload<CarpetFeatureParam>().text("config").required("feature").execute(
            [](CommandOrigin const&, CommandOutput& output, CarpetFeatureParam const& params) {
                CarpetCommand::handleConfig(params.feature);
            }
        );

        // config子命令 - 带feature和value参数
        command.overload<CarpetConfigParam>().text("config").required("feature").required("value").execute(
            [](CommandOrigin const&, CommandOutput& output, CarpetConfigParam const& params) {
                CarpetCommand::handleConfig(params.feature, params.value);
            }
        );

        mod->getLogger().info("Carpet command registered successfully");

    } catch (const std::exception& e) {
        mod->getLogger().error("Failed to register carpet command: {}", e.what());
    }
}

void CarpetCommand::handleHelp() {
    auto  mod    = ll::mod::NativeMod::current();
    auto& logger = mod->getLogger();

    logger.info("§6========== Carpet Help ==========");
    logger.info("§7Carpet Mod for LeviLamina - Bedrock Edition");
    logger.info("§7Usage: /carpet <subcommand> [args...]");
    logger.info("");
    logger.info("§6Available subcommands:");
    logger.info("§7  help - Show this help message");
    logger.info("§7  info - Show mod information");
    logger.info("§7  list - List available features");
    logger.info("§7  reload - Reload configuration (Admin only)");
    logger.info("§7  config [feature] [value] - Configure features");
    logger.info("");
    logger.info("§7For more help, visit: https://github.com/your-repo");
}

void CarpetCommand::handleInfo() {
    auto  mod    = ll::mod::NativeMod::current();
    auto& logger = mod->getLogger();

    logger.info("§6========== Carpet Info ==========");
    logger.info("§7Version: 1.0.0");
    logger.info("§7Game Version: 1.21.0+");
    logger.info("§7Author: Carpet Team");
    logger.info("");

    auto& configManager = ConfigManager::getInstance();
    auto& config        = configManager.getConfig();

    logger.info("§6Configuration:");
    logger.info("§7Language: {}", config.general.language);
    logger.info("§7Command Prefix: {}", config.commands.prefix);
    logger.info("§7Permission Level: {}", config.commands.permissionLevel);

    // TODO: 显示功能统计
    logger.info("§7Features Count: 0 (Coming soon)");
}

void CarpetCommand::handleList() {
    auto  mod    = ll::mod::NativeMod::current();
    auto& logger = mod->getLogger();

    logger.info("§6========== Available Features ==========");
    logger.info("");
    logger.info("§6Features:");
    logger.info("§7  cactus_wrench - Cactus wrench tool for debugging");
    logger.info("");
    logger.info("§7Usage: /carpet config <feature> <enable|disable>");
    logger.info("§7Example: /carpet config cactus_wrench enable");
}

void CarpetCommand::handleReload() {
    auto  mod    = ll::mod::NativeMod::current();
    auto& logger = mod->getLogger();

    logger.info("Reloading configuration...");

    // 重载配置
    bool configSuccess = ConfigManager::getInstance().reload();

    // 重载语言文件
    bool i18nSuccess = I18nManager::getInstance().reload();

    if (configSuccess && i18nSuccess) {
        logger.info("§aConfiguration reloaded successfully!");
    } else {
        if (!configSuccess) logger.error("§cFailed to reload configuration!");
        if (!i18nSuccess) logger.error("§cFailed to reload language files!");
    }
}

void CarpetCommand::handleConfig(const std::string& feature, const std::string& value) {
    auto  mod    = ll::mod::NativeMod::current();
    auto& logger = mod->getLogger();

    if (feature.empty()) {
        logger.info("§6Configuration Management");
        logger.info("§7Usage: /carpet config <feature> [enable|disable]");
        logger.info("§7Use '/carpet list' to see available features");
        return;
    }

    auto& configManager = ConfigManager::getInstance();

    // 如果只有功能名称，显示当前状态
    if (value.empty()) {
        bool        enabled = configManager.isFeatureEnabled(feature);
        std::string status  = enabled ? "§aEnabled" : "§cDisabled";
        logger.info("Feature '{}': {}", feature, status);
        return;
    }

    bool success = false;

    if (value == "enable" || value == "on" || value == "true" || value == "1") {
        success = configManager.enableFeature(feature);
        if (success) {
            logger.info("§aFeature '{}' has been enabled", feature);

            // 处理特殊功能
            if (feature == "cactus_wrench") {
                auto* module = ModuleManager::getModule("cactus_wrench");
                if (module && !module->isEnabled()) {
                    module->enable();
                }
            }
        }
    } else if (value == "disable" || value == "off" || value == "false" || value == "0") {
        success = configManager.disableFeature(feature);
        if (success) {
            logger.info("§cFeature '{}' has been disabled", feature);

            // 处理特殊功能
            if (feature == "cactus_wrench") {
                auto* module = ModuleManager::getModule("cactus_wrench");
                if (module && module->isEnabled()) {
                    module->disable();
                }
            }
        }
    } else {
        logger.error("§cInvalid value '{}'. Use 'enable' or 'disable'", value);
        return;
    }

    if (!success) {
        logger.error("§cFeature '{}' not found", feature);
    }
}

} // namespace carpet_mod_for_ll