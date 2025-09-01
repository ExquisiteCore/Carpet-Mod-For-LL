#include "BaseModule.h"
#include <chrono>
#include <vector>
#include <string>
#include <unordered_map>

namespace carpet_mod_for_ll {

// 性能统计数据
struct PerformanceData {
    double averageTPS = 20.0;
    double currentTPS = 20.0;
    double averageMSPT = 50.0; // 每tick毫秒数
    double currentMSPT = 50.0;
    
    // 内存使用
    size_t memoryUsed = 0;
    size_t memoryTotal = 0;
    
    // 实体统计
    int totalEntities = 0;
    std::unordered_map<std::string, int> entityCounts;
    
    // 区块统计
    int loadedChunks = 0;
    int activeChunks = 0;
    
    // 玩家统计
    int onlinePlayers = 0;
};

// 性能分析器模块
class ProfilerModule : public BaseModule {
private:
    PerformanceData perfData;
    std::vector<double> tpsHistory;
    std::vector<double> msptHistory;
    
    std::chrono::steady_clock::time_point lastTick;
    bool isRunning = false;
    int historySize = 100; // 保留100个tick的历史数据
    
public:
    ProfilerModule();
    ~ProfilerModule() override = default;
    
    // BaseModule interface
    bool onEnable() override;
    bool onDisable() override;
    void onTick() override;
    
    // 性能分析功能
    void startProfiling();
    void stopProfiling();
    void updatePerformanceData();
    void collectEntityStats();
    void collectChunkStats();
    void collectMemoryStats();
    
    // 数据访问
    const PerformanceData& getPerformanceData() const { return perfData; }
    std::vector<double> getTPSHistory() const { return tpsHistory; }
    std::vector<double> getMSPTHistory() const { return msptHistory; }
    
    // 报告生成
    std::string generateReport() const;
    std::string generateSimpleStatus() const;
    
    // 配置
    void setHistorySize(int size);
    bool isProfilingRunning() const { return isRunning; }
};

} // namespace carpet_mod_for_ll