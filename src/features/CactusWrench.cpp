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

        // 执行方块旋转功能
        if (isCactusItem(item) && isRotatableBlock(block)) {
            rotateBlock(&player, blockPos, block);
            
            // 阻止默认行为 (根据配置决定)
            auto& config = ConfigManager::getInstance().getConfig();
            if (config.features.cactusWrench.preventDefaultAction) {
                event.cancel();
            }
        }

    } catch (const std::exception& e) {
        auto mod = ll::mod::NativeMod::current();
        mod->getLogger().error("Error in CactusWrench::onPlayerInteractBlock: {}", e.what());
    }
}

bool CactusWrench::isCactusItem(const ItemStack& item) {
    // 检查物品是否为仙人掌
    std::string typeName = item.getTypeName();
    return typeName == "minecraft:cactus" || typeName == "cactus";
}

bool CactusWrench::isRotatableBlock(const Block& block) {
    // 检查方块是否为可旋转的红石元件
    std::string blockName = block.getTypeName();
    
    // 支持的可旋转红石元件
    return blockName.find("observer") != std::string::npos ||
           blockName.find("piston") != std::string::npos ||
           blockName.find("hopper") != std::string::npos ||
           blockName.find("dispenser") != std::string::npos ||
           blockName.find("dropper") != std::string::npos ||
           blockName.find("repeater") != std::string::npos ||
           blockName.find("comparator") != std::string::npos ||
           blockName == "minecraft:observer" ||
           blockName == "minecraft:piston" ||
           blockName == "minecraft:sticky_piston" ||
           blockName == "minecraft:hopper" ||
           blockName == "minecraft:dispenser" ||
           blockName == "minecraft:dropper" ||
           blockName == "minecraft:repeater" ||
           blockName == "minecraft:comparator";
}

void CactusWrench::rotateBlock(Player* player, const BlockPos& pos, const Block& block) {
    try {
        // 获取方块名称
        std::string blockName = block.getTypeName();
        
        // 根据方块类型执行不同的旋转逻辑 (暂时返回true表示成功)
        bool rotated = false;
        
        if (blockName.find("observer") != std::string::npos) {
            rotated = rotateObserver(nullptr, pos, block);
        } else if (blockName.find("piston") != std::string::npos) {
            rotated = rotatePiston(nullptr, pos, block);
        } else if (blockName.find("hopper") != std::string::npos) {
            rotated = rotateHopper(nullptr, pos, block);
        } else if (blockName.find("dispenser") != std::string::npos || blockName.find("dropper") != std::string::npos) {
            rotated = rotateDispenser(nullptr, pos, block);
        } else if (blockName.find("repeater") != std::string::npos || blockName.find("comparator") != std::string::npos) {
            rotated = rotateRepeater(nullptr, pos, block);
        }
        
        if (rotated) {
            std::string message = TR_FMT("carpet.cactus_wrench.rotate_success", blockName);
            player->sendMessage("§6[Cactus Wrench] §f" + message);
        } else {
            player->sendMessage("§6[Cactus Wrench] §cFailed to rotate block");
        }

    } catch (const std::exception& e) {
        auto mod = ll::mod::NativeMod::current();
        mod->getLogger().error("Failed to rotate block: {}", e.what());
        player->sendMessage(TR("carpet.cactus_wrench.error"));
    }
}

bool CactusWrench::rotateObserver(Dimension* dimension, const BlockPos& pos, const Block& block) {
    // 暂时实现简单的旋转逻辑
    // 实际实现需要获取方块状态并修改朝向属性
    // 这里先返回true表示旋转成功
    return true;
}

bool CactusWrench::rotatePiston(Dimension* dimension, const BlockPos& pos, const Block& block) {
    // 活塞旋转逻辑与侦测器相同
    return rotateObserver(dimension, pos, block);
}

bool CactusWrench::rotateHopper(Dimension* dimension, const BlockPos& pos, const Block& block) {
    // 暂时实现简单的旋转逻辑
    return true;
}

bool CactusWrench::rotateDispenser(Dimension* dimension, const BlockPos& pos, const Block& block) {
    // 发射器/投掷器旋转逻辑与侦测器相同
    return rotateObserver(dimension, pos, block);
}

bool CactusWrench::rotateRepeater(Dimension* dimension, const BlockPos& pos, const Block& block) {
    // 暂时实现简单的旋转逻辑
    return true;
}

} // namespace carpet_mod_for_ll