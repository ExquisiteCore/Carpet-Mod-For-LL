#pragma once

#include "../utils/I18nManager.h"
#include "BaseCommand.h"


namespace carpet_mod_for_ll {

class CarpetCommand : public BaseCommand {
public:
    CarpetCommand() : BaseCommand("carpet", TR("carpet.command.help"), CommandPermission::Any) {}

    // 重写注册命令方法以使用更好的参数处理
    bool registerCommand() override;

    // 显示帮助
    void showHelp(const CommandContext& ctx) const override;

private:
    void handleHelp(const CommandContext& ctx);
    void handleConfig(const CommandContext& ctx);
    void handleInfo(const CommandContext& ctx);
    void handleReload(const CommandContext& ctx);
    void handleList(const CommandContext& ctx);
};

} // namespace carpet_mod_for_ll