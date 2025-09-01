#pragma once

#include <filesystem>
#include <ll/api/i18n/I18n.h>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>


namespace carpet_mod_for_ll {

class I18nManager {
private:
    static std::unique_ptr<I18nManager>                                           instance;
    std::string                                                                   currentLocale;
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> translations;

public:
    I18nManager();
    ~I18nManager() = default;

    static I18nManager& getInstance();
    static void         initialize();
    static void         cleanup();

    // 获取翻译文本
    [[nodiscard]] std::string tr(const std::string& key) const;
    [[nodiscard]] std::string tr(const std::string& key, const std::vector<std::string>& args) const;

    // 设置语言
    void                      setLanguage(const std::string& lang);
    [[nodiscard]] std::string getCurrentLanguage() const;

    // 获取可用语言列表
    [[nodiscard]] std::vector<std::string> getAvailableLanguages() const;

    // 重新加载语言文件
    bool reload();

private:
    // 加载语言文件
    bool loadLanguageFile(const std::string& locale, const std::filesystem::path& filePath);

    // 格式化文本（支持 {} 占位符）
    [[nodiscard]] std::string formatText(const std::string& text, const std::vector<std::string>& args) const;
};

// 便捷宏定义
#define TR(key)          carpet_mod_for_ll::I18nManager::getInstance().tr(key)
#define TR_FMT(key, ...) carpet_mod_for_ll::I18nManager::getInstance().tr(key, {__VA_ARGS__})

} // namespace carpet_mod_for_ll