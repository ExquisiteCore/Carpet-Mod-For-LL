# Carpet Mod For LL - 项目架构文档

## 项目概述

Carpet Mod For LL 是基于 LeviLamina 平台开发的 Minecraft 基岩版地毯模组，参考了 trapdoor-ll 的设计理念，为技术玩家和服务器管理员提供强大的调试和管理工具。

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
├── functions/                  # 功能模块
│   ├── BaseModule.h            # 模块基类
│   ├── BaseModule.cpp          # 模块基类实现
│   ├── VillageModule.h         # 村庄功能模块
│   ├── VillageModule.cpp       # 村庄功能实现
│   ├── ProfilerModule.h        # 性能分析模块
│   └── ProfilerModule.cpp      # 性能分析实现
├── utils/                      # 工具类
│   ├── ConfigManager.h         # 配置管理器
│   ├── ConfigManager.cpp       # 配置管理实现
│   ├── I18nManager.h           # 国际化管理器
│   └── I18nManager.cpp         # 国际化管理实现
└── data/                       # 数据结构
    └── Position.h              # 位置和区域结构

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

#### 主要组件
- `Config`: 主配置结构，包含所有功能模块的配置
- `FeatureConfig`: 功能模块配置结构
- `ConfigManager`: 单例配置管理器

#### 使用方式
```cpp
// 获取配置
auto& config = CONFIG;

// 检查功能是否启用
if (IS_FEATURE_ENABLED("villageInfo")) {
    // 执行功能逻辑
}

// 启用/禁用功能
ConfigManager::getInstance().enableFeature("profiler");
```

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
std::string msg = TR_FMT("carpet.feature.enabled", {"villageInfo"});
```

### 3. 命令系统 (Command System)

#### 设计理念
- 模块化的子命令架构
- 权限级别控制
- 自动帮助生成
- 类型安全的参数处理

#### 主要组件
- `BaseCommand`: 命令基类
- `CommandContext`: 命令执行上下文
- `SubCommandInfo`: 子命令信息结构
- `CommandManager`: 命令管理器

#### 命令注册
```cpp
// 注册命令
CommandManager::registerCommand<CarpetCommand>();

// 添加子命令
addSubCommand({
    .name = "config",
    .description = "Configure features",
    .usage = "/carpet config <feature> [enable|disable]",
    .permission = CommandPermission::Admin,
    .handler = [this](const CommandContext& ctx) { handleConfig(ctx); }
});
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

#### 模块开发
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

## 已实现功能

### 1. 村庄模块 (VillageModule)
- 村庄信息扫描
- 村庄边界可视化
- 村庄统计数据
- 实时更新

### 2. 性能分析模块 (ProfilerModule)
- TPS 监控
- MSPT 统计
- 内存使用监控
- 实体统计
- 区块统计
- 性能报告生成

## 主要特性

### 类型安全
- 使用现代C++20特性
- 强类型的配置和数据结构
- RAII资源管理

### 模块化设计
- 松耦合的模块架构
- 可插拔的功能系统
- 清晰的职责分离

### 性能优化
- 智能的Tick系统
- 内存池和缓存机制
- 异步数据处理

### 用户友好
- 详细的错误信息
- 自动补全支持
- 多语言界面

## 扩展开发

### 添加新功能模块

1. 创建模块类继承 `BaseModule`
2. 实现 `onEnable()`, `onDisable()`, `onTick()` 方法
3. 在 `ModuleManager::initializeAllModules()` 中注册
4. 在 `Config.h` 中添加对应的配置项

### 添加新命令

1. 创建命令类继承 `BaseCommand`
2. 在构造函数中添加子命令
3. 实现子命令处理函数
4. 在 `CommandManager::registerAllCommands()` 中注册

### 添加多语言支持

1. 在 `assets/lang/` 目录添加语言文件
2. 使用 `TR()` 和 `TR_FMT()` 宏替换硬编码文本
3. 更新配置文件中的语言选项

## 构建和部署

### 构建命令
```bash
# 配置构建环境
xmake f -y -p windows -a x64 -m release

# 编译项目
xmake

# 清理构建
xmake clean
```

### 部署步骤
1. 编译生成 `.dll` 文件
2. 复制到 LeviLamina 的 `plugins/` 目录
3. 启动服务器，模组将自动加载

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
- 统一的命名约定
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

这个架构为未来的功能扩展提供了坚实的基础，同时保持了代码的可维护性和性能。