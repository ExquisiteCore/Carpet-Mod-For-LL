#pragma once

#include <string>

namespace carpet_mod_for_ll {

// Carpet命令参数结构
struct CarpetCommandParams {
    enum class Action {
        Help,
        Info, 
        List,
        Reload,
        Config
    };
    
    Action action;
    std::string feature;  // for config subcommand
    std::string value;    // for config subcommand
};

// Carpet命令管理类
class CarpetCommand {
public:
    // 注册命令到LeviLamina
    static void registerCommand();

private:
    // 命令处理函数
    static void handleHelp();
    static void handleInfo();
    static void handleList();
    static void handleReload();
    static void handleConfig(const std::string& feature, const std::string& value = "");
};

} // namespace carpet_mod_for_ll