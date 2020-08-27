
#ifndef GAME_GAME_H
#define GAME_GAME_H

#include "macro_magic/serializable.h"
#include "rendering/sprites/MegaSpriteSheet.h"

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
        FIELD_DEF_VAL(ivec2, windowSize, ivec2(1600, 900)),
        FIELD_DEF_VAL(bool, printOpenGLMessages, false),
        FIELD_DEF_VAL(bool, printOpenGLErrors, false),

        FIELD_DEF_VAL(bool, waterReflections, true),
        FIELD_DEF_VAL(bool, bloom, true),
        FIELD_DEF_VAL(bool, vignette, true),

        FIELD_DEF_VAL(bool, roomTransitionAnimation, true)
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
        FIELD_DEF_VAL(Key, fall, GLFW_KEY_S),

        FIELD_DEF_VAL(Key, nextTileShape, GLFW_KEY_RIGHT),
        FIELD_DEF_VAL(Key, prevTileShape, GLFW_KEY_LEFT),
        FIELD_DEF_VAL(Key, nextTileMaterial, GLFW_KEY_DOWN),
        FIELD_DEF_VAL(Key, prevTileMaterial, GLFW_KEY_UP),

        FIELD_DEF_VAL(Key, reloadAssets, GLFW_KEY_R),
        FIELD_DEF_VAL(Key, toggleFullscreen, GLFW_KEY_F11),
        FIELD_DEF_VAL(Key, toggleDeveloperOptions, GLFW_KEY_F3),

        FIELD_DEF_VAL(Key, inspectEntity, GLFW_KEY_I),
        FIELD_DEF_VAL(Key, moveEntity, GLFW_KEY_M)
    )
    END_SERIALIZABLE_FULL_JSON(KeyInputSettings)

    SERIALIZABLE(
        Settings,
        FIELD(GraphicsSettings, graphics),
        FIELD(AudioSettings, audio),
        FIELD(KeyInputSettings, keyInput),

        FIELD_DEF_VAL(bool, showDeveloperOptions, true)
    )
    END_SERIALIZABLE_FULL_JSON(Settings)

    extern Settings settings;

    void loadSettings();
    void saveSettings();

    extern MegaSpriteSheet spriteSheet;

};


#endif
