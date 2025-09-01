# Carpet Mod For LL - 项目架构文档

## 项目概述

Carpet Mod For LL 是基于 LeviLamina 平台开发的 Minecraft 基岩版地毯模组，参考了 trapdoor-ll 的设计理念，为技术玩家和服务器管理员提供强大的调试和管理工具。

项目当前处于架构完成阶段，所有核心系统已就绪，等待功能模块的具体实现。

## 核心架构

### 目录结构

```
src/
├── CarpetModForLl.cpp          # 主模组类
├── CarpetModForLl.h            # 主模组头文件
├── Config.h                    # 配置结构定义
├── Config.cpp                  # 配置结构实现
├── commands/                   # 命令系统
│   ├── BaseCommand.h           # 命令基类
│   ├── BaseCommand.cpp         # 命令基类实现
│   └── CarpetCommand.h         # 主命令实现
├── functions/                  # 功能模块框架
│   ├── BaseModule.h            # 模块基类
│   └── BaseModule.cpp          # 模块基类实现
├── utils/                      # 工具类
│   ├── ConfigManager.h         # 配置管理器
│   ├── ConfigManager.cpp       # 配置管理实现
│   ├── I18nManager.h           # 国际化管理器
│   └── I18nManager.cpp         # 国际化管理实现
└── data/                       # 数据结构 (待添加)

assets/
└── lang/                       # 语言文件
    ├── en_US.json             # 英文翻译
    └── zh_CN.json             # 中文翻译
```

## 核心系统

### 1. 配置系统 (Config System)

#### 设计理念
- 类型安全的配置结构
- 自动序列化/反序列化
- 运行时配置验证
- 热重载支持
- 模块化功能配置

#### 主要组件
- `Config`: 主配置结构，包含通用设置和功能配置占位
- `FeatureConfig`: 功能模块配置结构
- `ConfigManager`: 单例配置管理器

#### 使用方式
```cpp
// 获取配置
auto& config = CONFIG;

// 检查功能是否启用
if (IS_FEATURE_ENABLED("featureName")) {
    // 执行功能逻辑
}

// 启用/禁用功能
ConfigManager::getInstance().enableFeature("featureName");
```

#### 配置扩展
添加新功能配置需要：
1. 在 `Config.h` 的 `Features` 结构中添加 `FeatureConfig` 字段
2. 在 `Config.cpp` 的 `featureMap` 中添加映射关系
3. 在 `ConfigManager.cpp` 中设置默认状态

### 2. 国际化系统 (I18n System)

#### 设计理念
- 多语言支持
- 参数化文本模板
- 动态语言切换
- 自动语言检测

#### 主要组件
- `I18nManager`: 国际化管理器
- 语言文件 (JSON格式)
- 便捷宏定义

#### 使用方式
```cpp
// 基础翻译
std::string msg = TR("carpet.mod.name");

// 参数化翻译
std::string msg = TR_FMT("carpet.feature.enabled", {"featureName"});
```

#### 语言扩展
添加新语言支持需要：
1. 在 `assets/lang/` 目录创建对应的 `.json` 文件
2. 翻译所有现有的键值对
3. 在配置中设置语言选项

### 3. 命令系统 (Command System)

#### 设计理念
- 模块化的子命令架构
- 权限级别控制
- 自动帮助生成
- 类型安全的参数处理

#### 主要组件
- `BaseCommand`: 命令基类，提供子命令框架
- `CommandContext`: 命令执行上下文，包含发送者信息
- `SubCommandInfo`: 子命令信息结构
- `CommandManager`: 命令管理器

#### 核心命令 (CarpetCommand)
提供以下子命令：
- `help` - 显示帮助信息
- `config` - 配置功能开关
- `info` - 显示模组信息
- `reload` - 重载配置和语言文件
- `list` - 列出所有功能（当前为空）

#### 命令扩展
```cpp
// 创建新命令类
class MyCommand : public BaseCommand {
public:
    MyCommand() : BaseCommand("mycommand", "My command description") {
        addSubCommand({
            .name = "test",
            .description = "Test subcommand",
            .handler = [this](const CommandContext& ctx) { handleTest(ctx); }
        });
    }
};

// 注册命令
CommandManager::registerCommand<MyCommand>();
```

### 4. 功能模块系统 (Module System)

#### 设计理念
- 插件化的功能模块
- 统一的生命周期管理
- 自动的启用/禁用控制
- 性能优化的 Tick 系统

#### 主要组件
- `BaseModule`: 功能模块基类
- `ModuleManager`: 模块管理器
- `ModuleState`: 模块状态枚举

#### 模块生命周期
1. **注册阶段**: `ModuleManager::registerModule<T>()`
2. **初始化阶段**: `initializeAllModules()`
3. **配置同步**: `updateModulesFromConfig()`
4. **运行阶段**: `startTicking()` -> `onTick()`
5. **清理阶段**: `cleanup()`

#### 模块开发框架
```cpp
class MyModule : public BaseModule {
public:
    MyModule() : BaseModule("myModule", "My custom module") {}
    
    bool onEnable() override {
        // 模块启用逻辑
        return true;
    }
    
    bool onDisable() override {
        // 模块禁用逻辑
        return true;
    }
    
    void onTick() override {
        // 每tick执行的逻辑（可选）
    }
};

// 注册模块
ModuleManager::registerModule<MyModule>();
```

## 架构优势

### 1. 模块化设计
- 功能模块完全独立，互不影响
- 可插拔式架构，易于扩展和维护
- 配置驱动的功能控制

### 2. 类型安全
- 使用现代C++20特性
- 强类型的配置和数据结构
- RAII资源管理

### 3. 性能优化
- 智能的Tick系统，避免不必要的计算
- 统一的模块管理，减少系统开销
- 异常安全的错误处理

### 4. 用户友好
- 多语言界面支持
- 详细的错误信息和日志
- 直观的命令系统

### 5. 开发友好
- 清晰的代码结构和职责分离
- 完整的开发文档和示例
- 标准化的开发流程

## 当前状态

### ✅ 已完成
- 完整的架构设计
- 配置管理系统
- 国际化支持
- 命令框架
- 模块管理框架
- 基础文档

### ⏳ 待实现
- 具体的功能模块
- 数据结构定义
- API 集成
- 测试用例
- 性能优化

## 扩展开发指南

### 添加新功能模块

1. **创建模块文件**
   - `src/functions/YourModule.h` - 模块声明
   - `src/functions/YourModule.cpp` - 模块实现

2. **实现模块类**
   ```cpp
   class YourModule : public BaseModule {
       // 实现必要的虚函数
   };
   ```

3. **注册模块**
   在 `BaseModule.cpp` 的 `initializeAllModules()` 中添加：
   ```cpp
   registerModule<YourModule>();
   ```

4. **添加配置**
   在 `Config.h` 和 `Config.cpp` 中添加对应的配置项

5. **更新语言文件**
   在 `assets/lang/` 中添加相关的翻译文本

### 添加新命令

1. **创建命令类**
   继承 `BaseCommand` 并实现子命令

2. **注册命令**
   在主模组的 `enable()` 方法中注册

3. **更新帮助文档**
   确保命令有适当的描述和使用说明

### 数据结构设计

当需要添加数据结构时：
1. 在 `src/data/` 目录创建对应文件
2. 定义类型安全的数据结构
3. 提供必要的转换和操作方法
4. 考虑序列化需求

## 技术栈

- **语言**: C++20
- **构建系统**: XMake
- **依赖管理**: XMake Package Manager
- **主要依赖**:
  - LeviLamina 1.4.0+
  - GMLib
  - nlohmann/json (通过LeviLamina)

## 开发规范

### 代码风格
- 遵循现代C++最佳实践
- 使用智能指针管理内存
- 统一的命名约定 (驼峰命名)
- 详细的代码注释

### 错误处理
- 使用异常进行错误传播
- 详细的错误日志记录
- 优雅的错误恢复机制

### 性能考虑
- 避免不必要的内存分配
- 使用合适的数据结构
- 优化热路径代码
- 定期性能分析

这个架构为 Carpet Mod For LL 的功能实现提供了坚实的基础，既保证了代码的可维护性，又确保了良好的性能表现。所有的核心系统都已就绪，等待具体功能模块的实现。