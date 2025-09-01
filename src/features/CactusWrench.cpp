#include "CactusWrench.h"
#include "../utils/ConfigManager.h"
#include "../utils/I18nManager.h"

#include <ll/api/event/EventBus.h>
#include <ll/api/event/player/PlayerInteractBlockEvent.h>
#include <ll/api/mod/NativeMod.h>
#include <mc/world/level/BlockPos.h>
#include <mc/world/level/block/Block.h>
#include <mc/world/item/ItemStack.h>
#include <mc/world/actor/player/Player.h>
#include <mc/server/commands/CommandOrigin.h>
#include <mc/server/commands/CommandOutput.h>

namespace carpet_mod_for_ll {

CactusWrench::CactusWrench() : BaseModule("cactus_wrench", "Cactus Wrench functionality for hopper counter printing") {
}

bool CactusWrench::onEnable() {
    try {
        auto mod = ll::mod::NativeMod::current();
        
        // 使用正确的LeviLamina事件API注册监听器
        using namespace ll::event;
        EventBus& bus = EventBus::getInstance();
        
        mItemUseListener = bus.emplaceListener<PlayerInteractBlockEvent>(
            [this](PlayerInteractBlockEvent& event) {
                onPlayerInteractBlock(event);
            }
        );

        mod->getLogger().info("CactusWrench module enabled");
        return true;

    } catch (const std::exception& e) {
        auto mod = ll::mod::NativeMod::current();
        mod->getLogger().error("Failed to enable CactusWrench: {}", e.what());
        setError("Failed to register event listener: " + std::string(e.what()));
        return false;
    }
}

bool CactusWrench::onDisable() {
    try {
        // LeviLamina的ListenerPtr会自动管理生命周期，无需手动清理
        mItemUseListener.reset();

        auto mod = ll::mod::NativeMod::current();
        mod->getLogger().info("CactusWrench module disabled");
        clearError();
        return true;

    } catch (const std::exception& e) {
        auto mod = ll::mod::NativeMod::current();
        mod->getLogger().error("Failed to disable CactusWrench: {}", e.what());
        setError("Failed to unregister event listener: " + std::string(e.what()));
        return false;
    }
}

void CactusWrench::onPlayerInteractBlock(ll::event::PlayerInteractBlockEvent& event) {
    try {
        // 检查功能是否启用
        if (!isEnabled()) {
            return;
        }

        // 获取相关对象
        auto& player = event.self();
        auto& item = event.item();
        auto& blockPos = event.blockPos();
        auto blockOpt = event.block();
        
        // 检查方块是否有效
        if (!blockOpt.has_value()) {
            return;
        }
        
        auto& block = blockOpt.value();

        // 检查是否为仙人掌物品 + 混凝土方块的组合
        if (isCactusItem(item) && isConcreteBlock(block)) {
            // 执行漏斗计数器打印功能
            executeCounterPrint(&player, blockPos);
            
            // 阻止默认行为 (根据配置决定)
            auto& config = ConfigManager::getInstance().getConfig();
            if (config.features.cactusWrench.preventDefaultAction) {
                event.cancel();
            }
            
            auto mod = ll::mod::NativeMod::current();
            mod->getLogger().info("CactusWrench triggered by player {} at ({}, {}, {})", 
                                 player.getRealName(), blockPos.x, blockPos.y, blockPos.z);
        }

    } catch (const std::exception& e) {
        auto mod = ll::mod::NativeMod::current();
        mod->getLogger().error("Error in CactusWrench::onPlayerInteractBlock: {}", e.what());
    }
}

bool CactusWrench::isCactusItem(const ItemStack& item) {
    // 检查物品是否为仙人掌
    std::string typeName = item.getTypeName();
    auto mod = ll::mod::NativeMod::current();
    mod->getLogger().info("CactusWrench: Checking item type: {}", typeName);
    
    bool isCactus = typeName == "minecraft:cactus" || typeName == "cactus";
    mod->getLogger().info("CactusWrench: Item {} isCactus: {}", typeName, isCactus);
    
    return isCactus;
}

bool CactusWrench::isConcreteBlock(const Block& block) {
    // 检查方块是否为混凝土或侦测器
    std::string blockName = block.getTypeName();
    auto mod = ll::mod::NativeMod::current();
    mod->getLogger().info("CactusWrench: Checking block type: {}", blockName);
    
    // 支持混凝土方块 (排除混凝土粉末)
    bool isConcrete = blockName.find("concrete") != std::string::npos &&
                     blockName.find("powder") == std::string::npos;
    
    // 支持侦测器/观察者方块
    bool isDetector = blockName.find("observer") != std::string::npos ||
                     blockName.find("detector") != std::string::npos;
    
    mod->getLogger().info("CactusWrench: Block {} - isConcrete: {}, isDetector: {}", 
                         blockName, isConcrete, isDetector);
    
    return isConcrete || isDetector;
}

void CactusWrench::executeCounterPrint(Player* player, const BlockPos& pos) {
    try {
        auto mod = ll::mod::NativeMod::current();
        
        // 获取配置中的目标命令
        auto& config = ConfigManager::getInstance().getConfig();
        std::string command = formatCommand(config.features.cactusWrench.targetCommand, player, pos);
        
        mod->getLogger().info("CactusWrench: Player {} triggered counter print at ({}, {}, {})", 
                             player->getRealName(), pos.x, pos.y, pos.z);

        // 暂时通过消息通知玩家，而不是直接执行命令
        // 实际应该通过服务器命令系统执行，或者集成漏斗计数器功能
        std::string infoMessage = "§6[Cactus Wrench] §fHopper counter print at (" + 
                                 std::to_string(pos.x) + ", " + 
                                 std::to_string(pos.y) + ", " + 
                                 std::to_string(pos.z) + ")";
        player->sendMessage(infoMessage);
        
        // 发送调试命令信息给管理员
        if (player->getCommandPermissionLevel() >= CommandPermissionLevel::GameDirectors) {
            std::string cmdMessage = "§7[Debug] Would execute: §f" + command;
            player->sendMessage(cmdMessage);
        }
        
        // 发送成功消息给玩家
        std::string message = TR_FMT("carpet.cactus_wrench.success", std::to_string(pos.x), std::to_string(pos.y), std::to_string(pos.z));
        player->sendMessage(message);

    } catch (const std::exception& e) {
        auto mod = ll::mod::NativeMod::current();
        mod->getLogger().error("Failed to execute counter print: {}", e.what());
        
        // 向玩家发送错误消息
        player->sendMessage(TR("carpet.cactus_wrench.error"));
    }
}

std::string CactusWrench::formatCommand(const std::string& command, Player* player, const BlockPos& blockPos) {
    std::string result = command;
    
    // 替换坐标占位符
    auto replaceAll = [](std::string& str, const std::string& from, const std::string& to) {
        size_t pos = 0;
        while ((pos = str.find(from, pos)) != std::string::npos) {
            str.replace(pos, from.length(), to);
            pos += to.length();
        }
    };

    // 替换方块坐标
    replaceAll(result, "{bx}", std::to_string(blockPos.x));
    replaceAll(result, "{by}", std::to_string(blockPos.y));
    replaceAll(result, "{bz}", std::to_string(blockPos.z));
    
    // 替换玩家坐标
    auto playerPos = player->getPosition();
    replaceAll(result, "{px}", std::to_string(static_cast<int>(playerPos.x)));
    replaceAll(result, "{py}", std::to_string(static_cast<int>(playerPos.y)));
    replaceAll(result, "{pz}", std::to_string(static_cast<int>(playerPos.z)));

    return result;
}

} // namespace carpet_mod_for_ll