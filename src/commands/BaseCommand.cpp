#include "BaseCommand.h"
#include "../utils/I18nManager.h"
#include <ll/api/mod/NativeMod.h>
#include <ll/api/service/Service.h>
#include <ll/api/command/CommandRegistrar.h>
#include <ll/api/command/CommandHandle.h>
#include <mc/server/commands/CommandOrigin.h>
#include <mc/server/commands/CommandOutput.h>
#include <mc/server/commands/CommandPermissionLevel.h>
#include <mc/world/actor/player/Player.h>

namespace carpet_mod_for_ll {

std::vector<std::unique_ptr<BaseCommand>> CommandManager::commands;

// CommandContext 实现
bool CommandContext::isPlayer() const {
    auto* entity = origin->getEntity();
    if (entity == nullptr) {
        return false;
    }
    return entity->isPlayer();
}

bool CommandContext::isConsole() const {
    return origin->getOriginType() == CommandOriginType::DedicatedServer;
}

std::string CommandContext::getSenderName() const {
    if (isPlayer()) {
        return origin->getName();
    }
    return "Console";
}

bool CommandContext::hasPermission(CommandPermission level) const {
    auto permLevel = origin->getPermissionsLevel();
    return static_cast<int>(permLevel) >= static_cast<int>(level);
}

void CommandContext::success(const std::string& message) const {
    output->success(message);
}

void CommandContext::error(const std::string& message) const {
    output->error(message);
}

void CommandContext::info(const std::string& message) const {
    output->success(message); // LeviLamina 使用 success 作为普通消息输出
}

// BaseCommand 实现
BaseCommand::BaseCommand(const std::string& name, const std::string& desc, CommandPermission perm)
    : commandName(name), description(desc), defaultPermission(perm) {}

void BaseCommand::addSubCommand(const SubCommandInfo& subCmd) {
    subCommands.push_back(subCmd);
}

bool BaseCommand::registerCommand() {
    try {
        auto mod = ll::mod::NativeMod::current();
        
        // 使用 LeviLamina 的命令注册方式
        auto& command = ll::command::CommandRegistrar::getInstance()
                            .getOrCreateCommand(commandName, description, 
                                              static_cast<CommandPermissionLevel>(defaultPermission));
        
        // 注册主命令处理（处理无参数或help的情况）
        command.overload().execute([this](CommandOrigin const& origin, CommandOutput& output) {
            CommandContext ctx{&origin, &output, {}};
            execute(ctx);
        });
        
        // 为每个子命令注册一个重载
        for (const auto& subCmd : subCommands) {
            command.overload().text(subCmd.name).execute(
                [this, subCmdName = subCmd.name](CommandOrigin const& origin, CommandOutput& output) {
                    CommandContext ctx{&origin, &output, {subCmdName}};
                    execute(ctx);
                }
            );
        }
        
        mod->getLogger().info("Command '{}' registered successfully", commandName);
        return true;
        
    } catch (const std::exception& e) {
        auto mod = ll::mod::NativeMod::current();
        mod->getLogger().error("Failed to register command '{}': {}", commandName, e.what());
        return false;
    }
}

void BaseCommand::execute(const CommandContext& ctx) {
    // 检查权限
    if (!ctx.hasPermission(defaultPermission)) {
        ctx.error(TR("carpet.error.permission"));
        return;
    }
    
    // 如果没有参数或参数是 help，显示帮助
    if (ctx.args.empty() || ctx.args[0] == "help") {
        showHelp(ctx);
        return;
    }
    
    // 查找并执行子命令
    const auto* subCmd = findSubCommand(ctx.args[0]);
    if (!subCmd) {
        ctx.error(TR_FMT("carpet.error.invalid_args", ctx.args[0]));
        showHelp(ctx);
        return;
    }
    
    // 检查子命令权限
    if (!ctx.hasPermission(subCmd->permission)) {
        ctx.error(TR("carpet.error.permission"));
        return;
    }
    
    // 检查执行者类型限制
    if (subCmd->playerOnly && !ctx.isPlayer()) {
        ctx.error(TR("carpet.error.player_only"));
        return;
    }
    
    if (subCmd->consoleOnly && !ctx.isConsole()) {
        ctx.error(TR("carpet.error.console_only"));
        return;
    }
    
    // 执行子命令
    try {
        subCmd->handler(ctx);
    } catch (const std::exception& e) {
        ctx.error("Command execution failed: " + std::string(e.what()));
        auto mod = ll::mod::NativeMod::current();
        mod->getLogger().error("Exception in command '{}' subcommand '{}': {}", 
                             commandName, subCmd->name, e.what());
    }
}

void BaseCommand::showHelp(const CommandContext& ctx) const {
    ctx.info("§6========== " + commandName + " Help ==========");
    ctx.info("§7" + description);
    ctx.info("§7Usage: /" + commandName + " <subcommand> [args...]");
    ctx.info("");
    
    if (subCommands.empty()) {
        ctx.info("§cNo subcommands available");
        return;
    }
    
    ctx.info("§6Available subcommands:");
    for (const auto& sub : subCommands) {
        if (ctx.hasPermission(sub.permission)) {
            std::string line = "§7  " + sub.name;
            if (!sub.aliases.empty()) {
                line += " §8(";
                for (size_t i = 0; i < sub.aliases.size(); ++i) {
                    if (i > 0) line += ", ";
                    line += sub.aliases[i];
                }
                line += ")";
            }
            line += " §f- " + sub.description;
            ctx.info(line);
            
            if (!sub.usage.empty()) {
                ctx.info("§7    Usage: " + sub.usage);
            }
        }
    }
}

const SubCommandInfo* BaseCommand::findSubCommand(const std::string& name) const {
    for (const auto& sub : subCommands) {
        if (sub.name == name) {
            return &sub;
        }
        
        // 检查别名
        for (const auto& alias : sub.aliases) {
            if (alias == name) {
                return &sub;
            }
        }
    }
    return nullptr;
}

// CommandManager 实现
void CommandManager::registerAllCommands() {
    auto mod = ll::mod::NativeMod::current();
    mod->getLogger().info("Registering all commands...");
    
    // 这里后续会添加具体的命令注册
    // registerCommand<CarpetCommand>();
    // registerCommand<VillageCommand>();
    // registerCommand<ProfilerCommand>();
    // 等等...
    
    mod->getLogger().info("All commands registered successfully");
}

const std::vector<std::unique_ptr<BaseCommand>>& CommandManager::getAllCommands() {
    return commands;
}

void CommandManager::cleanup() {
    commands.clear();
}

} // namespace carpet_mod_for_ll