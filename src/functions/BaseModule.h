#pragma once

#include <functional>
#include <memory>
#include <string>
#include <vector>


// Forward declarations
namespace carpet_mod_for_ll {
class VillageModule;
class ProfilerModule;
} // namespace carpet_mod_for_ll

namespace carpet_mod_for_ll {

// 功能模块状态
enum class ModuleState {
    Disabled, // 禁用
    Enabled,  // 启用
    Error     // 错误状态
};

// 功能模块基类
class BaseModule {
protected:
    std::string moduleName;
    std::string description;
    ModuleState state;
    std::string lastError;

public:
    BaseModule(const std::string& name, const std::string& desc);
    virtual ~BaseModule() = default;

    // 基础接口
    virtual bool onEnable()  = 0;
    virtual bool onDisable() = 0;

    // 状态管理
    bool                      enable();
    bool                      disable();
    [[nodiscard]] ModuleState getState() const { return state; }
    [[nodiscard]] bool        isEnabled() const { return state == ModuleState::Enabled; }

    // 信息获取
    [[nodiscard]] const std::string& getName() const { return moduleName; }
    [[nodiscard]] const std::string& getDescription() const { return description; }
    [[nodiscard]] const std::string& getLastError() const { return lastError; }

protected:
    void setError(const std::string& error);
    void clearError();
};

// 功能模块管理器
class ModuleManager {
private:
    static std::vector<std::unique_ptr<BaseModule>> modules;

public:
    // 注册模块
    template <typename T, typename... Args>
    static void registerModule(Args&&... args) {
        modules.emplace_back(std::make_unique<T>(std::forward<Args>(args)...));
    }

    // 初始化所有模块
    static void initializeAllModules();

    // 根据配置启用/禁用模块
    static void updateModulesFromConfig();

    // 模块管理
    static BaseModule*              getModule(const std::string& name);
    static std::vector<BaseModule*> getAllModules();
    static std::vector<BaseModule*> getEnabledModules();

    // 启用/禁用模块
    static bool enableModule(const std::string& name);
    static bool disableModule(const std::string& name);

    // 清理
    static void cleanup();

    // 获取模块统计
    static size_t getTotalModuleCount();
    static size_t getEnabledModuleCount();
};

} // namespace carpet_mod_for_ll