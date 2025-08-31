#include "CarpetModForLl.h"


#include <ll/api/mod/RegisterHelper.h>

namespace carpet_mod_for_ll {

MyMod& MyMod::getInstance() {
    static MyMod instance;
    return instance;
}

bool MyMod::load() {
    getSelf().getLogger().debug("Loading...");
    // Code for loading the mod goes here.
    return true;
}

bool MyMod::enable() {
    getSelf().getLogger().debug("Enabling...");
    // Code for enabling the mod goes here.
    return true;
}

bool MyMod::disable() {
    getSelf().getLogger().debug("Disabling...");
    // Code for disabling the mod goes here.
    return true;
}

} // namespace carpet_mod_for_ll

LL_REGISTER_MOD(carpet_mod_for_ll::MyMod, carpet_mod_for_ll::MyMod::getInstance());
