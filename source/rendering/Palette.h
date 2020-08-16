
#ifndef GAME_PALETTE_H
#define GAME_PALETTE_H

#include <map>
#include <string>
#include <vector>
#include <utils/math_utils.h>
#include <asset_manager/asset.h>
#include <graphics/texture_array.h>

struct Palette
{

    std::vector<std::pair<std::string, vec3>> colors;

    Palette() = default;

    Palette(const char *path);

    void save(const char *path);

};

class Palettes3D
{
    SharedTexArray texture3D;
    bool changedByEditor = false;
    friend class PaletteEditor;

  public:
    const static int LIGHT_LEVELS = 4;

    struct Effect
    {
        std::string name;
        asset<Palette> lightLevels[LIGHT_LEVELS];
    };

    std::vector<Effect> effects;

    Palettes3D();

    /**
     * returns -1 when no effect was found for name
     */
    int effectIndex(const std::string &name) const;

    SharedTexArray get3DTexture();

  private:
    void create3DTexture();

};

#endif
