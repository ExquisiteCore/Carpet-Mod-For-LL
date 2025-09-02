# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [0.1.0] - 2024-09-02

### Added
- **Core Architecture**: Complete modular plugin system with BaseModule foundation
- **Command System**: Full `/carpet` command implementation with subcommands
  - `/carpet help` - Show help information
  - `/carpet info` - Display mod information and version
  - `/carpet list` - List all available features
  - `/carpet reload` - Reload configuration and language files
  - `/carpet config <feature> [enable|disable]` - Configure feature toggles
- **Configuration System**: JSON-based configuration with hot reloading
  - Feature-based configuration structure
  - Runtime configuration validation
  - Automatic default configuration generation
- **Internationalization**: Multi-language support system
  - Chinese (zh_CN) - Complete translation
  - English (en_US) - Complete translation
  - Parameterized text templates with TR_FMT macro
- **Cactus Wrench Feature**: Redstone component rotation tool
  - Right-click cactus on redstone components to rotate block facing
  - Support for Observer, Piston, Hopper, Dispenser, Dropper, Repeater, Comparator
  - Configurable enable/disable via commands
  - Visual feedback messages for successful operations

### Technical
- **Platform**: LeviLamina 1.4.0+ with GMLib integration
- **Language**: Modern C++20 with smart pointers and RAII
- **Build System**: XMake with automated dependency management
- **Architecture**: Plugin-based modular system with lifecycle management

### Dependencies
- LeviLamina 1.4.0+
- GMLib (automatic dependency)
- XMake build system

## [Unreleased]

### Planned Features
- Advanced redstone debugging tools
- Performance monitoring and profiling
- Fake player system
- Village analysis tools  
- Chunk analysis utilities
- HUD information display system