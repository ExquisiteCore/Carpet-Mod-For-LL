#include "CactusWrench.h"
#include "../utils/ConfigManager.h"
#include "../utils/I18nManager.h"

#include <ll/api/event/EventBus.h>
#include <ll/api/event/player/PlayerInteractBlockEvent.h>
#include <ll/api/mod/NativeMod.h>
#include <mc/world/level/BlockPos.h>
#include <mc/world/level/block/Block.h>
#include <mc/world/level/block/BlockLegacy.h>
#include <mc/world/level/BlockSource.h>
#include <mc/world/item/ItemStack.h>
#include <mc/world/actor/player/Player.h>
#include <mc/server/commands/CommandOrigin.h>
#include <mc/server/commands/CommandOutput.h>
#include <mc/nbt/CompoundTag.h>
#include <mc/nbt/IntTag.h>
#include <mc/nbt/StringTag.h>
#include <mc/nbt/ByteTag.h>

namespace carpet_mod_for_ll {

CactusWrench::CactusWrench() : BaseModule("cactus_wrench", "Cactus Wrench functionality for rotating blocks") {
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
        // 获取维度
        auto& dimension = player->getDimension();
        
        // 获取BlockSource
        auto& blockSource = dimension.getBlockSourceFromMainChunkSource();
        
        // 获取当前方块
        auto& currentBlock = blockSource.getBlock(pos);
        std::string blockName = currentBlock.getTypeName();
        
        // 尝试旋转方块
        bool rotated = performRotation(blockSource, pos, currentBlock);
        
        // 静默执行，不发送任何消息给玩家

    } catch (const std::exception& e) {
        auto mod = ll::mod::NativeMod::current();
        mod->getLogger().error("Failed to rotate block: {}", e.what());
        // 静默处理错误，不向玩家发送消息
    }
}

bool CactusWrench::performRotation(BlockSource& blockSource, const BlockPos& pos, const Block& block) {
    try {
        std::string typeName = block.getTypeName();
        
        // 移除minecraft:前缀
        if (typeName.find("minecraft:") == 0) {
            typeName = typeName.substr(10);
        }
        
        // 获取当前方块的数据值
        int variant = block.getData();
        int newVariant = variant;
        bool canRotate = false;
        
        // 根据方块类型决定旋转规则
        if (typeName == "observer" || typeName == "piston" || typeName == "sticky_piston" ||
            typeName == "dispenser" || typeName == "dropper") {
            // 6方向旋转
            newVariant = (variant + 1) % 6;
            canRotate = true;
        }
        else if (typeName == "hopper") {
            // 漏斗特殊处理: 跳过向上(1)
            if (variant == 0) newVariant = 2;      // down -> north
            else if (variant == 2) newVariant = 3; // north -> south
            else if (variant == 3) newVariant = 4; // south -> west
            else if (variant == 4) newVariant = 5; // west -> east
            else if (variant == 5) newVariant = 0; // east -> down
            else newVariant = 0; // 默认
            canRotate = true;
        }
        else if (typeName.find("stairs") != std::string::npos || 
                 typeName.find("trapdoor") != std::string::npos) {
            // 楼梯和活板门: variant的低3位控制朝向
            int direction = variant & 0x03;
            direction = (direction + 1) % 4;
            newVariant = (variant & ~0x03) | direction;
            canRotate = true;
        }
        else if (typeName == "repeater" || typeName == "unpowered_repeater" || typeName == "powered_repeater" ||
                 typeName == "comparator" || typeName == "unpowered_comparator" || typeName == "powered_comparator") {
            // 中继器和比较器: 低2位控制朝向
            int direction = variant & 0x03;
            direction = (direction + 1) % 4;
            newVariant = (variant & ~0x03) | direction;
            canRotate = true;
        }
        else if (typeName == "lever") {
            // 拉杆: 低3位控制朝向
            int direction = variant & 0x07;
            if (direction < 6) {
                direction = (direction + 1) % 6;
                newVariant = (variant & ~0x07) | direction;
                canRotate = true;
            }
        }
        else if (typeName == "torch" || typeName == "redstone_torch" || typeName == "unlit_redstone_torch") {
            // 火把: 1-5为墙上，0为地上
            if (variant > 0 && variant <= 5) {
                newVariant = (variant % 5) + 1;
                canRotate = true;
            }
        }
        else if (typeName.find("button") != std::string::npos) {
            // 按钮: 低3位控制朝向
            int direction = variant & 0x07;
            if (direction >= 1 && direction <= 5) {
                direction = (direction % 5) + 1;
                newVariant = (variant & ~0x07) | direction;
                canRotate = true;
            }
        }
        else if (typeName.find("_door") != std::string::npos) {
            // 门: 低2位控制朝向
            int direction = variant & 0x03;
            direction = (direction + 1) % 4;
            newVariant = (variant & ~0x03) | direction;
            canRotate = true;
        }
        else if (typeName.find("fence_gate") != std::string::npos) {
            // 栅栏门: 低2位控制朝向
            int direction = variant & 0x03;
            direction = (direction + 1) % 4;
            newVariant = (variant & ~0x03) | direction;
            canRotate = true;
        }
        else if (typeName == "anvil") {
            // 铁砧: 低2位控制朝向
            int direction = variant & 0x03;
            direction = (direction + 1) % 4;
            newVariant = (variant & ~0x03) | direction;
            canRotate = true;
        }
        else if (typeName == "lectern") {
            // 讲台: 低2位控制朝向
            int direction = variant & 0x03;
            direction = (direction + 1) % 4;
            newVariant = (variant & ~0x03) | direction;
            canRotate = true;
        }
        else if (typeName == "loom" || typeName == "stonecutter_block" || typeName == "grindstone") {
            // 织布机、切石机、砂轮
            int direction = variant & 0x03;
            direction = (direction + 1) % 4;
            newVariant = (variant & ~0x03) | direction;
            canRotate = true;
        }
        else if (typeName == "bell") {
            // 钟
            newVariant = (variant + 1) % 16;
            canRotate = true;
        }
        else if (typeName.find("rail") != std::string::npos) {
            // 铁轨
            if (typeName == "rail") {
                // 普通铁轨有10个方向
                newVariant = (variant + 1) % 10;
            } else {
                // 其他铁轨只有6个基础方向
                newVariant = (variant % 8 + 1) % 6 + (variant / 8) * 8;
            }
            canRotate = true;
        }
        else if (typeName.find("chest") != std::string::npos || typeName == "ender_chest") {
            // 箱子: 2-5为朝向
            if (variant >= 2 && variant <= 5) {
                newVariant = ((variant - 2 + 1) % 4) + 2;
                canRotate = true;
            }
        }
        else if (typeName == "furnace" || typeName == "lit_furnace" || 
                 typeName == "blast_furnace" || typeName == "lit_blast_furnace" ||
                 typeName == "smoker" || typeName == "lit_smoker") {
            // 熔炉类: 2-5为朝向
            if (variant >= 2 && variant <= 5) {
                newVariant = ((variant - 2 + 1) % 4) + 2;
                canRotate = true;
            }
        }
        
        if (canRotate && newVariant != variant) {
            // 创建新的方块
            auto newBlock = Block::tryGetFromRegistry(block.getTypeName(), static_cast<ushort>(newVariant));
            if (newBlock) {
                // 设置新方块
                blockSource.setBlock(pos, *newBlock, 3, nullptr, nullptr);
                return true;
            }
        }
        
        return false;
        
    } catch (const std::exception& e) {
        auto mod = ll::mod::NativeMod::current();
        mod->getLogger().error("Error in performRotation: {}", e.what());
        return false;
    }
}

} // namespace carpet_mod_for_ll