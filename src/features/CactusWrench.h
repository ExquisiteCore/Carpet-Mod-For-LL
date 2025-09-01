#pragma once

#include "../functions/BaseModule.h"
#include <ll/api/event/EventBus.h>
#include <ll/api/event/player/PlayerInteractBlockEvent.h>
#include <mc/world/item/ItemStack.h>
#include <mc/world/level/block/Block.h>
#include <mc/world/actor/player/Player.h>
#include <memory>

namespace carpet_mod_for_ll {

/**
 * 仙人掌扳手功能模块
 * 实现类似trapdoor-ll中的快捷键功能
 * 当玩家右键仙人掌对混凝土时，执行漏斗计数器打印功能
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

    // 检查是否为混凝土方块
    static bool isConcreteBlock(const Block& block);

    // 执行漏斗计数器打印功能
    void executeCounterPrint(Player* player, const BlockPos& pos);

    // 格式化命令字符串
    static std::string formatCommand(const std::string& command, Player* player, const BlockPos& blockPos);
};

} // namespace carpet_mod_for_ll