#include "CarpetModForLl.h"

#include "Config.h"
#include <ll/api/Config.h>
#include <ll/api/mod/RegisterHelper.h>

namespace carpet_mod_for_ll {
namespace {
Config config;
} // namespace

CarpetModForLl& CarpetModForLl ::getInstance() {
    static CarpetModForLl instance;
    return instance;
}

bool CarpetModForLl::load() {
    getSelf().getLogger().info("Loading...");
    // Code for loading the mod goes here.
    const auto& configFilePath = getSelf().getConfigDir() / "config.json";
    if (!ll::config::loadConfig(config, configFilePath)) {
        getSelf().getLogger().warn("Cannot load configurations from {}", configFilePath);
        getSelf().getLogger().info("Saving default configurations");

        if (!ll::config::saveConfig(config, configFilePath)) {
            getSelf().getLogger().error("Cannot save default configurations to {}", configFilePath);
        }
    }
    return true;
}

bool CarpetModForLl::enable() {
    getSelf().getLogger().debug("Enabling...");
    // Code for enabling the mod goes here.
    return true;
}

bool CarpetModForLl::disable() {
    getSelf().getLogger().debug("Disabling...");
    // Code for disabling the mod goes here.
    return true;
}

} // namespace carpet_mod_for_ll

LL_REGISTER_MOD(carpet_mod_for_ll::CarpetModForLl, carpet_mod_for_ll::CarpetModForLl::getInstance());
