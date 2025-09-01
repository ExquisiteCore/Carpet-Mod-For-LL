#include "BaseModule.h"
#include "../data/Position.h"
#include <vector>

namespace carpet_mod_for_ll {

struct VillageInfo {
    std::string name;
    Position center;
    int population;
    int beds;
    int workstations;
    bool hasIronGolem;
};

class VillageModule : public BaseModule {
private:
    std::vector<VillageInfo> villages;
    bool showBoundaries = false;
    bool showInfo = false;
    
public:
    VillageModule();
    ~VillageModule() override = default;
    
    // BaseModule interface
    bool onEnable() override;
    bool onDisable() override;
    void onTick() override;
    
    // Village functionality
    std::vector<VillageInfo> scanVillages();
    void updateVillageInfo();
    void displayVillageBoundaries(bool show);
    void displayVillageInfo(bool show);
    
    // Getters
    const std::vector<VillageInfo>& getVillages() const { return villages; }
    bool isBoundariesVisible() const { return showBoundaries; }
    bool isInfoVisible() const { return showInfo; }
};

} // namespace carpet_mod_for_ll