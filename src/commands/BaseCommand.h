#pragma once

#include <functional>
#include <string>
#include <vector>
#include <memory>

// 前向声明
class CommandOrigin;
class CommandOutput;

namespace carpet_mod_for_ll {

// 命令参数结构
struct IntParam {
    int value = 0;
};

// 命令权限级别 (与 CommandPermissionLevel 对应)
enum class CommandPermission {
    Any           = 0, // 任何人
    GameDirectors = 1, // 创造模式玩家
    Admin         = 2, // OP
    Host          = 3, // 控制台
    Owner         = 4, // 所有者
    Internal      = 5  // 内部
};

// 命令上下文
struct CommandContext {
    const CommandOrigin*     origin;
    CommandOutput*           output;
    std::vector<std::string> args;

    // 便捷方法
    [[nodiscard]] bool        isPlayer() const;
    [[nodiscard]] bool        isConsole() const;
    [[nodiscard]] std::string getSenderName() const;
    [[nodiscard]] bool        hasPermission(CommandPermission level) const;

    // 输出方法
    void success(const std::string& message) const;
    void error(const std::string& message) const;
    void info(const std::string& message) const;
};

// 子命令处理函数类型
using SubCommandHandler = std::function<void(const CommandContext&)>;

// 子命令信息
struct SubCommandInfo {
    std::string              name;
    std::string              description;
    std::string              usage;
    CommandPermission        permission = CommandPermission::Admin;
    SubCommandHandler        handler;
    std::vector<std::string> aliases;
    bool                     playerOnly  = false;
    bool                     consoleOnly = false;
};

// 基础命令类
class BaseCommand {
protected:
    std::string                 commandName;
    std::string                 description;
    CommandPermission           defaultPermission;
    std::vector<SubCommandInfo> subCommands;

public:
    BaseCommand(
        const std::string& name,
        const std::string& desc,
        CommandPermission  perm = CommandPermission::Admin
    );
    virtual ~BaseCommand() = default;

    // 添加子命令
    void addSubCommand(const SubCommandInfo& subCmd);

    // 注册命令
    virtual bool registerCommand();

    // 主命令处理函数
    virtual void execute(const CommandContext& ctx);

    // 显示帮助
    virtual void showHelp(const CommandContext& ctx) const;

    // 获取子命令
    [[nodiscard]] const SubCommandInfo* findSubCommand(const std::string& name) const;

    // 获取命令名称
    [[nodiscard]] const std::string& getName() const { return commandName; }
};

// 命令管理器
class CommandManager {
private:
    static std::vector<std::unique_ptr<BaseCommand>> commands;

public:
    // 注册命令
    template <typename T, typename... Args>
    static void registerCommand(Args&&... args) {
        commands.emplace_back(std::make_unique<T>(std::forward<Args>(args)...));
        commands.back()->registerCommand();
    }

    // 注册所有命令
    static void registerAllCommands();

    // 获取所有命令
    static const std::vector<std::unique_ptr<BaseCommand>>& getAllCommands();

    // 清理命令
    static void cleanup();
};

} // namespace carpet_mod_for_ll