
#ifndef GAME_GAME_H
#define GAME_GAME_H

#include <utils/cofu.h>

#include "../macro_magic/serializable.h"
#include "../rendering/sprites/MegaSpriteSheet.h"
#include "../rendering/Palette.h"
#include "session/Session.h"
#include "../rendering/ui/UIScreenManager.h"
#include "../generated/Game.hpp"

namespace Game {


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
