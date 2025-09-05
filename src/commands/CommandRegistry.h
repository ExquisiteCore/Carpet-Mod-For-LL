#pragma once

// 新的命令系统 - 不再需要BaseCommand抽象


namespace carpet_mod_for_ll {

// 命令注册管理器
class CommandRegistry {
public:
    // 注册所有命令
    static void registerAllCommands();
    
    // 清理所有命令
    static void cleanup();
};

} // namespace carpet_mod_for_ll