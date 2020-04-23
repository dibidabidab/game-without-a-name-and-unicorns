
#ifndef GAME_PALETTE_H
#define GAME_PALETTE_H

#include <map>
#include <string>
#include <utils/math_utils.h>

class Palette
{

  public:

    std::vector<std::pair<std::string, vec3>> colors;

    Palette() = default;

    Palette(const char *path);

};


#endif
