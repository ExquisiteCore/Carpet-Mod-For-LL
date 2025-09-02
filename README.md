# Carpet Mod For LL

一个基于 LeviLamina 平台开发的 Minecraft 基岩版地毯模组，为技术玩家和服务器管理员提供强大的调试和管理工具。

## 🌟 特性

- **🎯 模块化架构**: 灵活的插件式功能模块，可独立开关
- **🌐 多语言支持**: 支持中文、英文等多种语言
- **⚙️ 灵活配置**: 功能模块独立配置，运行时热重载
- **🎮 命令系统**: 直观的命令界面，权限控制，自动补全
- **🔧 仙人掌扳手**: 红石元件旋转工具

## 🚀 快速开始

### 环境要求

- **LeviLamina 1.4.0+**
- **GMLib** (自动依赖)
- **XMake** 构建系统
- **C++20** 编译器支持

### 构建步骤

1. **克隆项目**
   ```bash
   git clone https://github.com/ExquisiteCore/Carpet-Mod-For-LL.git
   cd Carpet-Mod-For-LL
   ```

2. **配置构建环境**
   ```bash
   xmake f -y -p windows -a x64 -m release
   ```

3. **编译项目**
   ```bash
   xmake
   ```

4. **部署模组**
   - 将生成的 `.dll` 文件复制到 LeviLamina 的 `plugins/` 目录
   - 重启服务器

## 📝 使用指南

### 基础命令

```bash
# 查看帮助
/carpet help

# 查看模组信息
/carpet info

# 列出所有功能
/carpet list

# 重载配置
/carpet reload

# 配置功能开关
/carpet config <功能名> [enable|disable]
```

### 🔧 仙人掌扳手功能

仙人掌扳手是本模组的核心功能，允许玩家快速旋转红石元件的朝向。

#### 使用方法
1. 手持仙人掌
2. 右键点击支持的红石元件
3. 方块朝向会旋转到下一个方向

#### 支持的方块
- **侦测器 (Observer)** - 6个朝向旋转
- **活塞 (Piston/Sticky Piston)** - 6个朝向旋转
- **漏斗 (Hopper)** - 5个朝向旋转
- **发射器 (Dispenser)** - 6个朝向旋转
- **投掷器 (Dropper)** - 6个朝向旋转
- **中继器 (Repeater)** - 4个水平方向旋转
- **比较器 (Comparator)** - 4个水平方向旋转

#### 配置选项
```bash
# 启用仙人掌扳手
/carpet config cactus_wrench enable

# 禁用仙人掌扳手
/carpet config cactus_wrench disable
```

## 🛠️ 配置说明

模组会在 `plugins/Carpet-Mod-For-LL/config.json` 创建配置文件：

```json
{
    "version": 1,
    "general": {
        "language": "zh_CN"
    },
    "commands": {
        "prefix": "carpet",
        "permissionLevel": 1
    },
    "features": {
        "cactusWrench": {
            "enabled": true,
            "preventDefaultAction": true,
            "targetCommand": "counter print {bx} {by} {bz}"
        }
    }
}
```

### 配置项说明

- `general.language`: 界面语言 (zh_CN/en_US)
- `commands.permissionLevel`: 命令权限等级
- `features.cactusWrench.enabled`: 是否启用仙人掌扳手
- `features.cactusWrench.preventDefaultAction`: 是否阻止默认的方块交互

## 🌐 多语言支持

- **中文 (zh_CN)**: 完整支持
- **English (en_US)**: 完整支持

语言文件位于 `assets/lang/` 目录，可以贡献更多语言翻译。

## 📊 项目状态

### ✅ 已完成功能
- 基础命令系统 (`/carpet help`, `/carpet info`, `/carpet list`, `/carpet reload`)
- 配置管理系统 (运行时配置切换)
- 国际化支持 (中英文)
- 仙人掌扳手功能 (红石元件旋转)

### 🚧 开发中
- 更多红石工具
- 性能监控工具
- 假玩家系统

### 📋 计划功能
- HUD 信息显示
- 村庄分析工具
- 区块分析工具
- 生成分析系统

## 🤝 贡献

欢迎贡献代码、报告问题或提出建议！

1. Fork 本项目
2. 创建功能分支 (`git checkout -b feature/AmazingFeature`)
3. 提交更改 (`git commit -m 'Add some AmazingFeature'`)
4. 推送到分支 (`git push origin feature/AmazingFeature`)
5. 创建 Pull Request

## 📄 许可证

本项目基于 MIT 许可证开源 - 查看 [LICENSE](LICENSE) 文件了解详情。

## 🙏 致谢

- [LeviLamina](https://github.com/LiteLDev/LeviLamina) - 模组平台
- [Carpet Mod](https://github.com/gnembon/fabric-carpet) - 设计灵感
- [trapdoor-ll](https://github.com/hhhxiao/trapdoor-ll) - 架构参考
```

### 当前状态

项目当前处于基础架构完成状态，包含：

✅ **完成的系统**:
- 配置管理系统
- 国际化支持
- 命令框架
- 模块管理框架

⏳ **待实现功能**:
- 所有 Carpet 功能模块都在计划中，详见 [FEATURES_TODO.md](FEATURES_TODO.md)

## ⚙️ 配置文件

配置文件位于 `plugins/Carpet-Mod-For-LL/config/config.json`:

```json
{
  "version": 1,
  "general": {
    "language": "zh_CN",
    "enableDebugLogging": false,
    "enableCommandLogging": true
  },
  "commands": {
    "prefix": "carpet",
    "permissionLevel": 2
  },
  "features": {
    // 功能配置将在实现时添加
  }
}
```

## 🏗️ 项目架构

项目采用模块化设计，主要组成部分：

- **配置系统**: 类型安全的配置管理，支持热重载
- **国际化系统**: 多语言支持，动态语言切换
- **命令系统**: 模块化子命令，权限控制
- **功能模块**: 插件化的功能实现，统一生命周期管理

详细架构文档请参考 [ARCHITECTURE.md](ARCHITECTURE.md)

## 🔧 开发指南

### 添加新功能模块

1. 创建模块类继承 `BaseModule`:

```cpp
class MyModule : public BaseModule {
public:
    MyModule() : BaseModule("myModule", "My custom module") {}
    
    bool onEnable() override {
        // 启用逻辑
        return true;
    }
    
    bool onDisable() override {
        // 禁用逻辑
        return true;
    }
};
```

2. 注册模块:

```cpp
// 在 BaseModule.cpp 中
ModuleManager::registerModule<MyModule>();
```

3. 添加配置项到 `Config.h`

### 功能开发计划

我们计划实现的所有功能详见 [FEATURES_TODO.md](FEATURES_TODO.md)，包括：

- 🏗️ **基础设施**: HUD显示、性能监控、日志系统
- 🤖 **自动化工具**: 假玩家、红石调试、世界控制
- 🌍 **世界分析**: 区块工具、村庄系统、生成分析
- 📊 **数据统计**: 实体管理、物品统计、玩家数据
- 🔧 **开发工具**: 方块信息、测试工具、网络分析

## 📊 开发进度

- ✅ **架构设计** - 完成
- ✅ **基础框架** - 完成
- ⏳ **功能模块** - 等待实现
- ⏳ **测试完善** - 待开始
- ⏳ **文档完善** - 进行中

## 🤝 参与贡献

我们欢迎所有形式的贡献！

### 如何开始
1. 查看 [FEATURES_TODO.md](FEATURES_TODO.md) 选择感兴趣的功能
2. Fork 项目
3. 创建特性分支 (`git checkout -b feature/amazing-feature`)
4. 实现功能并添加测试
5. 提交更改 (`git commit -m 'Add amazing feature'`)
6. 推送到分支 (`git push origin feature/amazing-feature`)
7. 开启 Pull Request

### 代码规范

- 使用现代 C++20 特性
- 遵循项目命名约定
- 添加适当的错误处理
- 编写清晰的文档和注释

## 🙋 常见问题

### Q: 为什么功能列表是空的？
A: 项目当前专注于建立稳固的架构基础。所有 Carpet 功能都在开发计划中，您可以参与实现！

### Q: 如何参与开发？
A: 查看 [FEATURES_TODO.md](FEATURES_TODO.md) 了解计划功能，选择感兴趣的模块开始贡献。

### Q: 项目的目标是什么？
A: 为 Minecraft 基岩版提供类似 Java 版 Carpet Mod 的完整功能集，专为技术玩家和服务器管理员设计。

## 📄 许可证

本项目基于 MIT 许可证开源 - 查看 [LICENSE](LICENSE) 文件了解详情。

## 🎯 致谢

- 感谢 [trapdoor-ll](https://github.com/bedrock-dev/trapdoor-ll) 项目提供的设计灵感
- 感谢 [LeviLamina](https://github.com/LiteLDev/LeviLamina) 团队提供的优秀框架
- 感谢所有贡献者的付出

## 📞 联系方式

- 项目主页: [GitHub](https://github.com/ExquisiteCore/Carpet-Mod-For-LL)
- 问题反馈: [Issues](https://github.com/ExquisiteCore/Carpet-Mod-For-LL/issues)

---

🌟 如果这个项目对你有帮助，请给我们一个 Star！
