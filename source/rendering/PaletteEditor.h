
#ifndef GAME_PALETTEEDITOR_H
#define GAME_PALETTEEDITOR_H


#include <vector>
#include <asset_manager/asset.h>
#include "Palette.h"

class PaletteEditor
{
    std::string selectedEffect;

  public:
    void drawGUI();

  private:

    void drawPalettes(std::vector<asset<Palette>> &);

    void revert(asset<Palette> &);
    void revert(asset<Palette> &, int colorIndex);
};


#endif
