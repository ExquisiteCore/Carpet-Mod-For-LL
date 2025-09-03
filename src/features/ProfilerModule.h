#pragma once

#include "../functions/BaseModule.h"
#include <chrono>
#include <map>
#include <string>

namespace carpet_mod_for_ll {

/**
 * 性能分析模块
 * 实现服务器性能监控和分析
 */
class ProfilerModule : public BaseModule {
public:
    ProfilerModule();
    ~ProfilerModule() override = default;

    bool onEnable() override;
    bool onDisable() override;

    // 性能分析功能
    void startProfiling(int ticks, const std::string& mode);
    void stopProfiling();
    void showProfileResults();

    // 分析模式
    void profileNormal(int ticks);
    void profileChunk(int ticks);
    void profileEntity(int ticks);
    void profilePendingTick(int ticks);
    void profileMSPT(int ticks);

    // Hook回调接口（供TickHookManager调用）
    bool isProfiling() const { return isProfiling_; }
    void recordTickTime(std::chrono::microseconds time);
    void recordChunkTime(int chunkX, int chunkZ, std::chrono::microseconds time);
    void recordEntityTime(const std::string& entityType, std::chrono::microseconds time);
    void recordRedstoneTime(std::chrono::microseconds time);

private:
    enum class ProfileMode { None, Normal, Chunk, Entity, PendingTick, MSPT };

    struct ProfileData {
        std::chrono::microseconds totalTime{0};
        size_t                    sampleCount{0};
        std::chrono::microseconds minTime{std::chrono::microseconds::max()};
        std::chrono::microseconds maxTime{0};

        void   addSample(std::chrono::microseconds time);
        double getAverageMs() const;
    };

    struct ChunkProfileData {
        std::map<std::pair<int, int>, ProfileData> chunkTimes; // <chunkX, chunkZ> -> ProfileData
        ProfileData                                randomTickTime;
        ProfileData                                blockEntityTickTime;
        ProfileData                                pendingTickTime;
    };

    struct EntityProfileData {
        std::map<std::string, ProfileData> entityTypes; // EntityType -> ProfileData
        size_t                             totalEntityCount = 0;
    };

    ProfileMode currentMode  = ProfileMode::None;
    bool        isProfiling_ = false;
    int         targetTicks  = 0;
    int         currentTick  = 0;

    // 分析数据
    ProfileData                           overallData;
    ChunkProfileData                      chunkData;
    EntityProfileData                     entityData;
    std::map<std::pair<int, int>, size_t> pendingTickCounts; // <chunkX, chunkZ> -> count

    // 辅助方法
    void resetProfileData();
    void printNormalResults();
    void printChunkResults();
    void printEntityResults();
    void printPendingTickResults();
    void printMSPTResults();
};

} // namespace carpet_mod_for_ll