#include "CarpetCommand.h"
#include "../utils/ConfigManager.h"
#include "../utils/I18nManager.h"
#include <ll/api/mod/NativeMod.h>
#include <ll/api/service/Service.h>
#include <ll/api/command/CommandRegistrar.h>
#include <ll/api/command/CommandHandle.h>
#include <mc/server/commands/CommandOrigin.h>
#include <mc/server/commands/CommandOutput.h>
#include <mc/server/commands/CommandPermissionLevel.h>

namespace carpet_mod_for_ll {

// 命令参数结构
struct CarpetCommandParams {
    std::string subcommand;
    std::string feature;
    std::string action;
};

bool CarpetCommand::registerCommand() {
    try {
        auto mod = ll::mod::NativeMod::current();
        
        // 使用 LeviLamina 的命令注册方式
        auto& command = ll::command::CommandRegistrar::getInstance()
                            .getOrCreateCommand("carpet", "Carpet Mod main command", 
                                              CommandPermissionLevel::Any);
        
        // 无参数命令 - 显示帮助
        command.overload().execute([this](CommandOrigin const& origin, CommandOutput& output) {
            CommandContext ctx{&origin, &output, {}};
            showHelp(ctx);
        });
        
        // help 子命令
        command.overload().text("help").execute([this](CommandOrigin const& origin, CommandOutput& output) {
            CommandContext ctx{&origin, &output, {"help"}};
            handleHelp(ctx);
        });
        
        // info 子命令
        command.overload().text("info").execute([this](CommandOrigin const& origin, CommandOutput& output) {
            CommandContext ctx{&origin, &output, {"info"}};
            handleInfo(ctx);
        });
        
        // reload 子命令 (需要管理员权限)
        command.overload().text("reload").execute([this](CommandOrigin const& origin, CommandOutput& output) {
            // 检查执行者是否有权限
            if (origin.getPermissionsLevel() < CommandPermissionLevel::Admin) {
                output.error("Only administrators can use this command");
                return;
            }
            
            CommandContext ctx{&origin, &output, {"reload"}};
            handleReload(ctx);
        });
        
        // list 子命令
        command.overload().text("list").execute([this](CommandOrigin const& origin, CommandOutput& output) {
            CommandContext ctx{&origin, &output, {"list"}};
            handleList(ctx);
        });
        
        mod->getLogger().info("Command 'carpet' registered successfully");
        return true;
        
    } catch (const std::exception& e) {
        auto mod = ll::mod::NativeMod::current();
        mod->getLogger().error("Failed to register command 'carpet': {}", e.what());
        return false;
    }
}

void CarpetCommand::handleHelp(const CommandContext& ctx) {
    showHelp(ctx);
}

void CarpetCommand::handleConfig(const CommandContext& ctx) {
    if (ctx.args.size() < 2) {
        ctx.error("Usage: /carpet config <feature> [enable|disable]");
        return;
    }
    
    std::string featureName = ctx.args[1];
    auto& configManager = ConfigManager::getInstance();
    
    // 如果只有功能名称，显示当前状态
    if (ctx.args.size() == 2) {
        bool enabled = configManager.isFeatureEnabled(featureName);
        ctx.info("Feature '" + featureName + "' is currently " + 
                (enabled ? "§aenabled" : "§cdisabled"));
        return;
    }
    
    std::string action = ctx.args[2];
    bool success = false;
    
    if (action == "enable" || action == "on" || action == "true") {
        success = configManager.enableFeature(featureName);
        if (success) {
            ctx.success(TR_FMT("carpet.feature.enabled", featureName));
        }
    } else if (action == "disable" || action == "off" || action == "false") {
        success = configManager.disableFeature(featureName);
        if (success) {
            ctx.success(TR_FMT("carpet.feature.disabled", featureName));
        }
    } else {
        ctx.error("Invalid action. Use 'enable' or 'disable'");
        return;
    }
    
    if (!success) {
        ctx.error(TR_FMT("carpet.feature.not_found", featureName));
    }
}

void CarpetCommand::handleInfo(const CommandContext& ctx) {
    ctx.info("§6========== Carpet Mod For LL ==========");
    ctx.info(TR_FMT("carpet.info.version", "1.0.0", "1.21.0"));
    ctx.info(TR("carpet.info.author"));
    ctx.info("");
    
    auto& configManager = ConfigManager::getInstance();
    auto& config = configManager.getConfig();
    
    ctx.info("§6Configuration:");
    ctx.info("  Language: §f" + config.general.language);
    ctx.info("  Command Prefix: §f" + config.commands.prefix);
    ctx.info("  Permission Level: §f" + std::to_string(config.commands.permissionLevel));
    
    // 功能统计 - 暂时显示为0，功能将在后续版本中添加
    ctx.info("§6Features: §f0/0 enabled (Features will be added in future updates)");
}

void CarpetCommand::handleReload(const CommandContext& ctx) {
    ctx.info("Reloading configuration and language files...");
    
    // 重载配置
    bool configSuccess = ConfigManager::getInstance().reload();
    
    // 重载语言文件
    bool i18nSuccess = I18nManager::getInstance().reload();
    
    if (configSuccess && i18nSuccess) {
        ctx.success("All files reloaded successfully");
    } else {
        if (!configSuccess) ctx.error("Failed to reload configuration");
        if (!i18nSuccess) ctx.error("Failed to reload language files");
    }
}

void CarpetCommand::handleList(const CommandContext& ctx) {
    ctx.info("§6========== Available Features ==========");
    ctx.info(TR("carpet.info.no_features"));
    ctx.info("");
    ctx.info("§7Features will be added in future updates...");
    ctx.info("");
    ctx.info("§7Use '/carpet config <feature> enable/disable' to toggle features");
}

void CarpetCommand::showHelp(const CommandContext& ctx) const {
    ctx.info("§6========== Carpet Help ==========");
    ctx.info("§7Carpet Mod for LeviLamina - Technical enhancements for Minecraft Bedrock");
    ctx.info("§7Usage: /carpet <subcommand> [args...]");
    ctx.info("");
    
    ctx.info("§6Available subcommands:");
    ctx.info("§7  help §f- Show this help message");
    ctx.info("§7  info §f- Show mod information and version");
    ctx.info("§7  list §f- List all available features");
    ctx.info("§7  reload §8(Admin) §f- Reload configuration and language files");
    ctx.info("");
    ctx.info("§7Use '/carpet <subcommand>' for detailed help on each command");
}

} // namespace carpet_mod_for_ll