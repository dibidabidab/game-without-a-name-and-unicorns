
#ifndef GAME_GAME_H
#define GAME_GAME_H

#include <utils/cofu.h>

#include "../macro_magic/serializable.h"
#include "../rendering/sprites/MegaSpriteSheet.h"
#include "../rendering/Palette.h"
#include "SaveGame.h"

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

        FIELD_DEF_VAL(bool, roomTransitionAnimation, true),

        FIELD_DEF_VAL(u8vec3, imGuiThemeColor_background, vec3(37, 33, 49)),
        FIELD_DEF_VAL(u8vec3, imGuiThemeColor_text, vec3(244, 241, 222)),
        FIELD_DEF_VAL(u8vec3, imGuiThemeColor_main, vec3(218, 17, 94)),
        FIELD_DEF_VAL(u8vec3, imGuiThemeColor_mainAccent, vec3(121, 35, 89)),
        FIELD_DEF_VAL(u8vec3, imGuiThemeColor_highLight, vec3(199, 239, 0))
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
        FIELD_DEF_VAL(Key, moveEntity, GLFW_KEY_M),
        FIELD_DEF_VAL(Key, moveEntityAndSpawnPoint, GLFW_KEY_LEFT_ALT),
        FIELD_DEF_VAL(Key, createEntity, GLFW_KEY_INSERT)
    )
    END_SERIALIZABLE_FULL_JSON(KeyInputSettings)

    SERIALIZABLE(
        Settings,
        FIELD(GraphicsSettings, graphics),
        FIELD(AudioSettings, audio),
        FIELD(KeyInputSettings, keyInput),
        FIELD_DEF_VAL(json, customSettings, json::object()), // (will be) used by lua scripts

        FIELD_DEF_VAL(bool, showDeveloperOptions, true)
    )
    END_SERIALIZABLE_FULL_JSON(Settings)

    extern Settings settings;

    void loadSettings();
    void saveSettings();

    extern MegaSpriteSheet spriteSheet;
    extern cofu<Palettes3D> palettes;

    SaveGame &getSaveGame();
    SaveGame *tryGetSaveGame();

    void loadOrCreateSaveGame(const char *path);
    void saveSaveGame(const char *path=NULL); // when no path is provided, the path given in loadOrCreateSaveGame() is used.

    void unloadSaveGame();

};


#endif
