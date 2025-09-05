#pragma once

#include "mc/server/commands/CommandOutput.h"
#include <string>

namespace carpet_mod_for_ll {


// Carpet命令管理类
class CarpetCommand {
public:
    // 注册命令到LeviLamina
    static void registerCommand();

private:
    // 命令处理函数
    static void handleHelp(CommandOutput& output);
    static void handleInfo(CommandOutput& output);
    static void handleList(CommandOutput& output);
    static void handleReload(CommandOutput& output);
    static void handleConfig(CommandOutput& output, const std::string& feature, const std::string& value = "");
};

} // namespace carpet_mod_for_ll