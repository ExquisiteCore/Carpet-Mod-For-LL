#pragma once

#include "../functions/BaseModule.h"
#include <ll/api/event/EventBus.h>
#include <ll/api/event/player/PlayerInteractBlockEvent.h>
#include <mc/world/actor/player/Player.h>
#include <mc/world/item/ItemStack.h>
#include <mc/world/level/BlockSource.h>
#include <mc/world/level/block/Block.h>
#include <mc/world/level/dimension/Dimension.h>

namespace carpet_mod_for_ll {

/**
 * 仙人掌扳手功能模块
 * 实现红石元件旋转功能
 * 当玩家右键仙人掌对红石元件时，旋转方块的朝向
 */
class CactusWrench : public BaseModule {
public:
    CactusWrench();
    ~CactusWrench() override = default;

    bool onEnable() override;
    bool onDisable() override;

private:
    // 事件监听器 - 使用正确的LeviLamina API
    ll::event::ListenerPtr mItemUseListener;

    // 处理玩家交互方块事件
    void onPlayerInteractBlock(ll::event::PlayerInteractBlockEvent& event);

    // 检查是否为仙人掌物品
    static bool isCactusItem(const ItemStack& item);

    // 检查是否为可旋转的红石元件
    static bool isRotatableBlock(const Block& block);

    // 执行方块旋转功能
    void rotateBlock(Player* player, const BlockPos& pos, const Block& block);

    // 执行实际的旋转操作
    static bool performRotation(BlockSource& blockSource, const BlockPos& pos, const Block& block);
};

} // namespace carpet_mod_for_ll