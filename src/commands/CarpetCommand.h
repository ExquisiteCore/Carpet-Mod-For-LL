#include "BaseCommand.h"
#include "../utils/ConfigManager.h"
#include "../utils/I18nManager.h"

namespace carpet_mod_for_ll {

class CarpetCommand : public BaseCommand {
public:
    CarpetCommand() : BaseCommand("carpet", TR("carpet.command.help"), CommandPermission::GameMaster) {
        setupSubCommands();
    }

private:
    void setupSubCommands() {
        // help 子命令
        addSubCommand({
            .name = "help",
            .description = TR("carpet.command.help"),
            .usage = "/carpet help [command]",
            .permission = CommandPermission::Everyone,
            .handler = [this](const CommandContext& ctx) { handleHelp(ctx); }
        });
        
        // config 子命令
        addSubCommand({
            .name = "config",
            .description = TR("carpet.command.config"),
            .usage = "/carpet config <feature> [enable|disable]",
            .permission = CommandPermission::Admin,
            .handler = [this](const CommandContext& ctx) { handleConfig(ctx); },
            .aliases = {"cfg", "setting"}
        });
        
        // info 子命令
        addSubCommand({
            .name = "info",
            .description = TR("carpet.command.info"),
            .usage = "/carpet info",
            .permission = CommandPermission::Everyone,
            .handler = [this](const CommandContext& ctx) { handleInfo(ctx); },
            .aliases = {"version", "about"}
        });
        
        // reload 子命令
        addSubCommand({
            .name = "reload",
            .description = "Reload configuration and language files",
            .usage = "/carpet reload",
            .permission = CommandPermission::Admin,
            .handler = [this](const CommandContext& ctx) { handleReload(ctx); },
            .consoleOnly = false
        });
        
        // list 子命令
        addSubCommand({
            .name = "list",
            .description = "List all available features",
            .usage = "/carpet list",
            .permission = CommandPermission::GameMaster,
            .handler = [this](const CommandContext& ctx) { handleList(ctx); },
            .aliases = {"features"}
        });
    }
    
    void handleHelp(const CommandContext& ctx) {
        showHelp(ctx);
    }
    
    void handleConfig(const CommandContext& ctx) {
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
    
    void handleInfo(const CommandContext& ctx) {
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
        
        // 统计启用的功能
        auto featureNames = config.getAllFeatureNames();
        int enabledCount = 0;
        for (const auto& name : featureNames) {
            if (config.isFeatureEnabled(name)) {
                enabledCount++;
            }
        }
        
        ctx.info("§6Features: §f" + std::to_string(enabledCount) + "/" + 
                std::to_string(featureNames.size()) + " enabled");
    }
    
    void handleReload(const CommandContext& ctx) {
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
    
    void handleList(const CommandContext& ctx) {
        auto& config = ConfigManager::getInstance().getConfig();
        auto featureNames = config.getAllFeatureNames();
        
        if (featureNames.empty()) {
            ctx.info("§6========== Available Features ==========");
            ctx.info(TR("carpet.info.no_features"));
            ctx.info("");
            ctx.info("§7Features will be added in future updates...");
            return;
        }
        
        ctx.info("§6========== Available Features ==========");
        ctx.info(TR_FMT("carpet.info.features_total", {std::to_string(featureNames.size()), "0"}));
        ctx.info("");
        
        for (const auto& name : featureNames) {
            bool enabled = config.isFeatureEnabled(name);
            const auto* feature = config.getFeature(name);
            
            std::string status = enabled ? "§a✓" : "§c✗";
            std::string line = status + " §f" + name;
            
            if (feature && !feature->description.empty()) {
                line += " §7- " + feature->description;
            }
            
            ctx.info(line);
        }
        
        ctx.info("");
        ctx.info("§7Use '/carpet config <feature> enable/disable' to toggle features");
    }
};

} // namespace carpet_mod_for_ll