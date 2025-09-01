#pragma once

#include <ll/api/i18n/I18n.h>
#include <memory>
#include <string>

namespace carpet_mod_for_ll {

class I18nManager {
private:
    static std::unique_ptr<I18nManager> instance;
    ll::i18n::I18n i18n;
    
public:
    I18nManager();
    ~I18nManager() = default;
    
    static I18nManager& getInstance();
    static void initialize();
    static void cleanup();
    
    // 获取翻译文本
    std::string tr(const std::string& key) const;
    std::string tr(const std::string& key, const std::vector<std::string>& args) const;
    
    // 设置语言
    void setLanguage(const std::string& lang);
    std::string getCurrentLanguage() const;
    
    // 获取可用语言列表
    std::vector<std::string> getAvailableLanguages() const;
    
    // 重新加载语言文件
    bool reload();
};

// 便捷宏定义
#define TR(key) carpet_mod_for_ll::I18nManager::getInstance().tr(key)
#define TR_FMT(key, ...) carpet_mod_for_ll::I18nManager::getInstance().tr(key, {__VA_ARGS__})

} // namespace carpet_mod_for_ll