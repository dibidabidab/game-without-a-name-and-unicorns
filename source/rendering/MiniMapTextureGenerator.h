
#ifndef GAME_MINIMAPTEXTUREGENERATOR_H
#define GAME_MINIMAPTEXTUREGENERATOR_H

#include <graphics/texture.h>
#include "../level/Level.h"

class MiniMapTextureGenerator
{

  public:

    static SharedTexture generate(const Level &);

};


#endif
