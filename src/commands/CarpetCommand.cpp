#include "CarpetCommand.h"
#include "../utils/ConfigManager.h"
#include "../utils/I18nManager.h"
#include "../functions/BaseModule.h"
#include <ll/api/mod/NativeMod.h>
#include <ll/api/service/Service.h>
#include <ll/api/command/CommandRegistrar.h>
#include <ll/api/command/CommandHandle.h>
#include <mc/server/commands/CommandOrigin.h>
#include <mc/server/commands/CommandOutput.h>
#include <mc/server/commands/CommandPermissionLevel.h>
#include <mc/world/actor/player/Player.h>

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
                output.error(TR("carpet.error.admin_only"));
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
        
        // config 子命令 (无参数)
        command.overload().text("config").execute([this](CommandOrigin const& origin, CommandOutput& output) {
            CommandContext ctx{&origin, &output, {"config"}};
            handleConfig(ctx);
        });
        
        // config <feature> 子命令
        command.overload<CarpetCommandParams>()
            .text("config")
            .required("feature", &CarpetCommandParams::feature)
            .execute([this](CommandOrigin const& origin, CommandOutput& output, const CarpetCommandParams& params) {
                CommandContext ctx{&origin, &output, {"config", params.feature}};
                handleConfig(ctx);
            });
            
        // config <feature> <action> 子命令
        command.overload<CarpetCommandParams>()
            .text("config")
            .required("feature", &CarpetCommandParams::feature)
            .required("action", &CarpetCommandParams::action)
            .execute([this](CommandOrigin const& origin, CommandOutput& output, const CarpetCommandParams& params) {
                CommandContext ctx{&origin, &output, {"config", params.feature, params.action}};
                handleConfig(ctx);
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
        ctx.error(TR("carpet.config.usage.detailed"));
        return;
    }
    
    std::string featureName = ctx.args[1];
    auto& configManager = ConfigManager::getInstance();
    
    // 如果只有功能名称，显示当前状态
    if (ctx.args.size() == 2) {
        bool enabled = configManager.isFeatureEnabled(featureName);
        std::string status = enabled ? TR("carpet.config.feature.enabled_status") : TR("carpet.config.feature.disabled_status");
        ctx.info(TR_FMT("carpet.config.feature.status", featureName, status));
        return;
    }
    
    std::string action = ctx.args[2];
    bool success = false;
    
    if (action == "enable" || action == "on" || action == "true" || action == "启用") {
        success = configManager.enableFeature(featureName);
        if (success) {
            ctx.success(TR_FMT("carpet.feature.enabled", featureName));
            
            // 如果是cactus_wrench，动态启用模块
            if (featureName == "cactus_wrench") {
                auto* module = ModuleManager::getModule("cactus_wrench");
                if (module && !module->isEnabled()) {
                    module->enable();
                }
            }
        }
    } else if (action == "disable" || action == "off" || action == "false" || action == "禁用") {
        success = configManager.disableFeature(featureName);
        if (success) {
            ctx.success(TR_FMT("carpet.feature.disabled", featureName));
            
            // 如果是cactus_wrench，动态禁用模块
            if (featureName == "cactus_wrench") {
                auto* module = ModuleManager::getModule("cactus_wrench");
                if (module && module->isEnabled()) {
                    module->disable();
                }
            }
        }
    } else {
        ctx.error(TR("carpet.config.action.invalid"));
        return;
    }
    
    if (!success) {
        ctx.error(TR_FMT("carpet.feature.not_found", featureName));
    }
}

void CarpetCommand::handleInfo(const CommandContext& ctx) {
    ctx.info(TR("carpet.info.title"));
    ctx.info(TR_FMT("carpet.info.version", "1.0.0", "1.21.0"));
    ctx.info(TR("carpet.info.author"));
    ctx.info("");
    
    auto& configManager = ConfigManager::getInstance();
    auto& config = configManager.getConfig();
    
    ctx.info(TR("carpet.info.config"));
    ctx.info(TR("carpet.info.language") + config.general.language);
    ctx.info(TR("carpet.info.prefix") + config.commands.prefix);
    ctx.info(TR("carpet.info.permission") + std::to_string(config.commands.permissionLevel));
    
    // 功能统计 - 暂时显示为0，功能将在后续版本中添加
    ctx.info(TR_FMT("carpet.info.features_count", "0"));
}

void CarpetCommand::handleReload(const CommandContext& ctx) {
    ctx.info(TR("carpet.reload.start"));
    
    // 重载配置
    bool configSuccess = ConfigManager::getInstance().reload();
    
    // 重载语言文件
    bool i18nSuccess = I18nManager::getInstance().reload();
    
    if (configSuccess && i18nSuccess) {
        ctx.success(TR("carpet.reload.success"));
    } else {
        if (!configSuccess) ctx.error(TR("carpet.reload.config_failed"));
        if (!i18nSuccess) ctx.error(TR("carpet.reload.i18n_failed"));
    }
}

void CarpetCommand::handleList(const CommandContext& ctx) {
    ctx.info(TR("carpet.list.title"));
    ctx.info("");
    
    // 显示仙人掌扳手功能
    ctx.info("§6Available Features:");
    ctx.info("§7  cactus_wrench §f- " + TR("carpet.cactus_wrench.feature.description"));
    ctx.info("");
    ctx.info(TR("carpet.list.usage"));
}

void CarpetCommand::showHelp(const CommandContext& ctx) const {
    ctx.info(TR("carpet.help.title"));
    ctx.info(TR("carpet.help.description"));
    ctx.info(TR("carpet.help.usage"));
    ctx.info("");
    
    ctx.info(TR("carpet.help.subcommands"));
    ctx.info("§7  " + TR("carpet.help.subcmd.help"));
    ctx.info("§7  " + TR("carpet.help.subcmd.info"));
    ctx.info("§7  " + TR("carpet.help.subcmd.list"));
    ctx.info("§7  " + TR("carpet.help.subcmd.reload"));
    ctx.info("§7  " + TR("carpet.help.subcmd.config"));
    ctx.info("");
    ctx.info("§7" + TR("carpet.help.footer"));
}

} // namespace carpet_mod_for_ll