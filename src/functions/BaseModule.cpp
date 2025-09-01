#include "BaseModule.h"
#include "VillageModule.h"
#include "ProfilerModule.h"
#include "../utils/ConfigManager.h"
#include <ll/api/mod/NativeMod.h>
#include <ll/api/schedule/Scheduler.h>

namespace carpet_mod_for_ll {

std::vector<std::unique_ptr<BaseModule>> ModuleManager::modules;
bool ModuleManager::tickEnabled = false;

// BaseModule 实现
BaseModule::BaseModule(const std::string& name, const std::string& desc)
    : moduleName(name), description(desc), state(ModuleState::Disabled) {}

bool BaseModule::enable() {
    if (state == ModuleState::Enabled) {
        return true; // 已经启用
    }
    
    try {
        clearError();
        if (onEnable()) {
            state = ModuleState::Enabled;
            auto& mod = ll::mod::NativeMod::current();
            mod.getLogger().info("Module '{}' enabled successfully", moduleName);
            return true;
        } else {
            state = ModuleState::Error;
            auto& mod = ll::mod::NativeMod::current();
            mod.getLogger().warn("Module '{}' failed to enable: {}", moduleName, lastError);
            return false;
        }
    } catch (const std::exception& e) {
        setError(e.what());
        state = ModuleState::Error;
        auto& mod = ll::mod::NativeMod::current();
        mod.getLogger().error("Exception enabling module '{}': {}", moduleName, e.what());
        return false;
    }
}

bool BaseModule::disable() {
    if (state == ModuleState::Disabled) {
        return true; // 已经禁用
    }
    
    try {
        clearError();
        if (onDisable()) {
            state = ModuleState::Disabled;
            auto& mod = ll::mod::NativeMod::current();
            mod.getLogger().info("Module '{}' disabled successfully", moduleName);
            return true;
        } else {
            auto& mod = ll::mod::NativeMod::current();
            mod.getLogger().warn("Module '{}' failed to disable: {}", moduleName, lastError);
            return false;
        }
    } catch (const std::exception& e) {
        setError(e.what());
        auto& mod = ll::mod::NativeMod::current();
        mod.getLogger().error("Exception disabling module '{}': {}", moduleName, e.what());
        return false;
    }
}

void BaseModule::setError(const std::string& error) {
    lastError = error;
}

void BaseModule::clearError() {
    lastError.clear();
}

// ModuleManager 实现
void ModuleManager::initializeAllModules() {
    auto& mod = ll::mod::NativeMod::current();
    mod.getLogger().info("Initializing all modules...");
    
    // 注册所有功能模块
    registerModule<VillageModule>();
    registerModule<ProfilerModule>();
    // 后续添加更多模块...
    
    mod.getLogger().info("All modules initialized. Total: {}", modules.size());
    
    // 根据配置启用模块
    updateModulesFromConfig();
}

void ModuleManager::updateModulesFromConfig() {
    auto& configManager = ConfigManager::getInstance();
    
    for (auto& module : modules) {
        bool shouldBeEnabled = configManager.isFeatureEnabled(module->getName());
        
        if (shouldBeEnabled && !module->isEnabled()) {
            module->enable();
        } else if (!shouldBeEnabled && module->isEnabled()) {
            module->disable();
        }
    }
}

BaseModule* ModuleManager::getModule(const std::string& name) {
    for (auto& module : modules) {
        if (module->getName() == name) {
            return module.get();
        }
    }
    return nullptr;
}

std::vector<BaseModule*> ModuleManager::getAllModules() {
    std::vector<BaseModule*> result;
    result.reserve(modules.size());
    
    for (auto& module : modules) {
        result.push_back(module.get());
    }
    
    return result;
}

std::vector<BaseModule*> ModuleManager::getEnabledModules() {
    std::vector<BaseModule*> result;
    
    for (auto& module : modules) {
        if (module->isEnabled()) {
            result.push_back(module.get());
        }
    }
    
    return result;
}

bool ModuleManager::enableModule(const std::string& name) {
    auto* module = getModule(name);
    if (module) {
        bool success = module->enable();
        if (success) {
            // 更新配置
            ConfigManager::getInstance().enableFeature(name);
        }
        return success;
    }
    return false;
}

bool ModuleManager::disableModule(const std::string& name) {
    auto* module = getModule(name);
    if (module) {
        bool success = module->disable();
        if (success) {
            // 更新配置
            ConfigManager::getInstance().disableFeature(name);
        }
        return success;
    }
    return false;
}

void ModuleManager::startTicking() {
    if (tickEnabled) return;
    
    tickEnabled = true;
    
    // 设置定时器进行tick处理
    ll::schedule::repeat([]() {
        if (tickEnabled) {
            onTick();
        }
    }, 1); // 每tick执行一次
    
    auto& mod = ll::mod::NativeMod::current();
    mod.getLogger().info("Module ticking started");
}

void ModuleManager::stopTicking() {
    tickEnabled = false;
    
    auto& mod = ll::mod::NativeMod::current();
    mod.getLogger().info("Module ticking stopped");
}

void ModuleManager::onTick() {
    for (auto& module : modules) {
        if (module->isEnabled()) {
            try {
                module->onTick();
            } catch (const std::exception& e) {
                auto& mod = ll::mod::NativeMod::current();
                mod.getLogger().error("Exception in module '{}' tick: {}", 
                                    module->getName(), e.what());
                // 可以考虑暂时禁用出错的模块
            }
        }
    }
}

void ModuleManager::cleanup() {
    stopTicking();
    
    // 禁用所有模块
    for (auto& module : modules) {
        if (module->isEnabled()) {
            module->disable();
        }
    }
    
    modules.clear();
}

size_t ModuleManager::getTotalModuleCount() {
    return modules.size();
}

size_t ModuleManager::getEnabledModuleCount() {
    size_t count = 0;
    for (const auto& module : modules) {
        if (module->isEnabled()) {
            count++;
        }
    }
    return count;
}

} // namespace carpet_mod_for_ll