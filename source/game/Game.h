
#ifndef GAME_GAME_H
#define GAME_GAME_H

#include <utils/cofu.h>

#include "../rendering/sprites/MegaSpriteSheet.h"
#include "../rendering/Palette.h"
#include "../rendering/ui/UIScreenManager.h"
#include "../generated/GameSettings.hpp"

namespace Game {

    extern Settings settings;

    void loadSettings();

    void saveSettings();

    extern MegaSpriteSheet spriteSheet;
    extern cofu<Palettes3D> palettes;

    extern cofu<UIScreenManager> uiScreenManager;

};


#endif
