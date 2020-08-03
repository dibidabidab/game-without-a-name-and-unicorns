
#ifndef GAME_GAME_H
#define GAME_GAME_H

#include "macro_magic/serializable.h"

#ifdef EMSCRIPTEN
#define V_SYNC_ENABLED_BY_DEFAULT false
#else
#define V_SYNC_ENABLED_BY_DEFAULT true
#endif

namespace Game
{

    SERIALIZABLE(
        GraphicsSettings,
        FIELD_DEF_VAL(bool, vsync, V_SYNC_ENABLED_BY_DEFAULT),
        FIELD_DEF_VAL(bool, fullscreen, false),
        FIELD_DEF_VAL(ivec2, windowSize, ivec2(1600, 900))
    )
    END_SERIALIZABLE_FULL_JSON(GraphicsSettings)

    SERIALIZABLE(
        AudioSettings,
        FIELD_DEF_VAL(float, masterVolume, 1.)
    )
    END_SERIALIZABLE_FULL_JSON(AudioSettings)

    using namespace KeyInput;
    SERIALIZABLE(
        KeyInputSettings,
        FIELD_DEF_VAL(Key, walkLeft, GLFW_KEY_A),
        FIELD_DEF_VAL(Key, walkRight, GLFW_KEY_D),
        FIELD_DEF_VAL(Key, jump, GLFW_KEY_SPACE),
        FIELD_DEF_VAL(Key, fall, GLFW_KEY_S)
    )
    END_SERIALIZABLE_FULL_JSON(KeyInputSettings)

    SERIALIZABLE(
        Settings,
        FIELD(GraphicsSettings, graphics),
        FIELD(AudioSettings, audio),
        FIELD(KeyInputSettings, keyInput)
    )
    END_SERIALIZABLE_FULL_JSON(Settings)

    extern Settings settings;

    void loadSettings();
    void saveSettings();

};


#endif
