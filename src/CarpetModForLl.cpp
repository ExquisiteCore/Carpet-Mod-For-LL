#include "CarpetModForLl.h"


#include <ll/api/mod/RegisterHelper.h>

namespace carpet_mod_for_ll {

CarpetModForLl& CarpetModForLl ::getInstance() {
    static CarpetModForLl instance;
    return instance;
}

bool CarpetModForLl::load() {
    getSelf().getLogger().debug("Loading...");
    // Code for loading the mod goes here.
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
