#include "CarpetModForLl.h"

#include "commands/BaseCommand.h"
#include "commands/CarpetCommand.h"
#include "commands/ProfCommand.h"
#include "commands/TickCommand.h"
#include "features/CactusWrench.h"
#include "features/ProfilerModule.h"
#include "features/TickModule.h"
#include "functions/BaseModule.h"
#include "utils/ConfigManager.h"
#include "utils/I18nManager.h"
#include <ll/api/Config.h>
#include <ll/api/mod/RegisterHelper.h>


namespace carpet_mod_for_ll {

CarpetModForLl& CarpetModForLl::getInstance() {
    static CarpetModForLl instance;
    return instance;
}

bool CarpetModForLl::load() {
    getSelf().getLogger().info("Loading Carpet Mod For LL...");

    try {
        // 初始化 I18n 系统
        I18nManager::initialize();
        getSelf().getLogger().info("I18n system initialized");

        // 初始化配置管理器
        ConfigManager::initialize();
        getSelf().getLogger().info("Configuration system initialized");

        getSelf().getLogger().info("Carpet Mod For LL loaded successfully");
        return true;

    } catch (const std::exception& e) {
        getSelf().getLogger().error("Failed to load Carpet Mod For LL: {}", e.what());
        return false;
    }
}

bool CarpetModForLl::enable() {
    getSelf().getLogger().info("Enabling Carpet Mod For LL...");

    try {
        // 先注册功能模块
        ModuleManager::registerModule<CactusWrench>();
        ModuleManager::registerModule<TickModule>();
        ModuleManager::registerModule<ProfilerModule>();
        getSelf().getLogger().info("Modules registered");

        // 初始化功能模块
        ModuleManager::initializeAllModules();
        getSelf().getLogger().info("Modules initialized");

        // 注册命令
        CommandManager::registerCommand<CarpetCommand>();
        CommandManager::registerCommand<TickCommand>();
        CommandManager::registerCommand<ProfCommand>();
        getSelf().getLogger().info("Commands registered");

        getSelf().getLogger().info("Carpet Mod For LL enabled successfully");
        return true;

    } catch (const std::exception& e) {
        getSelf().getLogger().error("Failed to enable Carpet Mod For LL: {}", e.what());
        return false;
    }
}

bool CarpetModForLl::disable() {
    getSelf().getLogger().info("Disabling Carpet Mod For LL...");

    try {
        // 清理模块系统
        ModuleManager::cleanup();
        getSelf().getLogger().info("Modules cleaned up");

        // 清理命令系统
        CommandManager::cleanup();
        getSelf().getLogger().info("Commands cleaned up");

        // 清理配置管理器
        ConfigManager::cleanup();
        getSelf().getLogger().info("Configuration system cleaned up");

        // 清理 I18n 系统
        I18nManager::cleanup();
        getSelf().getLogger().info("I18n system cleaned up");

        getSelf().getLogger().info("Carpet Mod For LL disabled successfully");
        return true;

    } catch (const std::exception& e) {
        getSelf().getLogger().error("Failed to disable Carpet Mod For LL: {}", e.what());
        return false;
    }
}

} // namespace carpet_mod_for_ll

LL_REGISTER_MOD(carpet_mod_for_ll::CarpetModForLl, carpet_mod_for_ll::CarpetModForLl::getInstance());
