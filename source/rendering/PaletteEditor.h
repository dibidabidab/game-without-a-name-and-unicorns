
#ifndef GAME_PALETTEEDITOR_H
#define GAME_PALETTEEDITOR_H


#include <vector>
#include <asset_manager/asset.h>
#include "Palette.h"

class PaletteEditor
{
    std::string selectedEffect;

  public:
    bool show = true;

    void drawGUI(Palettes3D &palettes);

  private:

    bool drawPalettes(Palettes3D::Effect &);

    void revert(asset<Palette> &);
    void revert(asset<Palette> &, int colorIndex);
};


#endif
