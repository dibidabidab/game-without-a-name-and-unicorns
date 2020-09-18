
#ifndef GAME_GAME_H
#define GAME_GAME_H

#include <utils/cofu.h>

#include "../macro_magic/serializable.h"
#include "../rendering/sprites/MegaSpriteSheet.h"
#include "../rendering/Palette.h"
#include "session/Session.h"
#include "../rendering/ui/UIScreenManager.h"
#include "../generated/Game.hpp"

#ifdef EMSCRIPTEN
#define V_SYNC_ENABLED_BY_DEFAULT false
#else
#define V_SYNC_ENABLED_BY_DEFAULT true
#endif

namespace Game {
    using namespace KeyInput;


    extern Settings settings;

    void loadSettings();

    void saveSettings();

    extern std::map<std::string, std::string> startupArgs;

    extern MegaSpriteSheet spriteSheet;
    extern cofu<Palettes3D> palettes;

    extern cofu<UIScreenManager> uiScreenManager;

    extern delegate<void()> onSessionChange;

    Session &getCurrentSession();

    Session *tryGetCurrentSession();

    void setCurrentSession(Session *);

};


#endif
