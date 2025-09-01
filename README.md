# Carpet Mod For LL

一个基于 LeviLamina 平台开发的 Minecraft 基岩版地毯模组，为技术玩家和服务器管理员提供强大的调试和管理工具。

## 🌟 特性

- **🏘️ 村庄系统**: 村庄信息显示、边界可视化、统计分析
- **📊 性能分析**: TPS监控、内存使用统计、实体和区块分析
- **🌐 多语言支持**: 支持中文、英文等多种语言
- **⚙️ 灵活配置**: 功能模块独立开关，运行时热重载
- **🎯 命令系统**: 直观的命令界面，权限控制，自动补全

## 🚀 快速开始

### 环境要求

- **LeviLamina 1.4.0+**
- **GMLib** (自动依赖)
- **XMake** 构建系统
- **C++20** 编译器支持

### 构建步骤

1. **克隆项目**
   ```bash
   git clone https://github.com/your-repo/Carpet-Mod-For-LL.git
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

# 启用功能
/carpet config villageInfo enable
/carpet config profiler enable

# 查看模组信息
/carpet info

# 列出所有功能
/carpet list

# 重载配置
/carpet reload
```

### 功能模块

#### 🏘️ 村庄分析
- **功能**: 扫描和分析服务器中的村庄
- **用法**: `/carpet config villageInfo enable`
- **特性**: 
  - 村庄边界显示
  - 村民统计
  - 工作站和床位分析
  - 铁傀儡状态

#### 📊 性能分析器
- **功能**: 监控服务器性能指标
- **用法**: `/carpet config profiler enable`
- **特性**:
  - 实时TPS监控
  - 内存使用统计
  - 实体和区块统计
  - 性能报告生成

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
    "villageInfo": {
      "enabled": false,
      "description": "Show village information"
    },
    "profiler": {
      "enabled": false,
      "description": "Server performance profiler"
    }
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

### 添加新命令

1. 继承 `BaseCommand` 类
2. 实现子命令处理逻辑
3. 在 `CommandManager` 中注册

### 多语言支持

在 `assets/lang/` 目录添加对应的语言文件，使用 `TR()` 宏替换硬编码文本。

## 📊 性能优化

- **智能Tick系统**: 避免不必要的计算
- **内存管理**: 使用智能指针，避免内存泄漏
- **异步处理**: 重要操作异步执行
- **缓存机制**: 合理缓存计算结果

## 🤝 参与贡献

1. Fork 项目
2. 创建特性分支 (`git checkout -b feature/amazing-feature`)
3. 提交更改 (`git commit -m 'Add amazing feature'`)
4. 推送到分支 (`git push origin feature/amazing-feature`)
5. 开启 Pull Request

### 代码规范

- 使用现代 C++20 特性
- 遵循项目命名约定
- 添加适当的错误处理
- 编写单元测试

## 📋 待办事项

- [ ] 假玩家系统
- [ ] HUD 显示系统
- [ ] 红石调试工具
- [ ] 方块信息显示
- [ ] 实体追踪系统
- [ ] 区块加载工具
- [ ] 漏斗计数器
- [ ] 史莱姆区块查找器

## 🙋 常见问题

### Q: 模组加载失败怎么办？
A: 检查 LeviLamina 版本是否兼容，查看服务器日志了解具体错误信息。

### Q: 如何更新配置？
A: 可以直接编辑配置文件，然后使用 `/carpet reload` 命令重载。

### Q: 功能启用后没有效果？
A: 确认权限等级足够，检查模组日志是否有错误信息。

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
