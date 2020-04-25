
#include "ShadowCaster.h"

ShadowCaster::ShadowCaster() : fbo(TEXTURE_SIZE, TEXTURE_SIZE)
{
    fbo.addColorTexture(GL_R8UI, GL_RED_INTEGER, GL_NEAREST, GL_NEAREST);
}

void ShadowCaster::tileMapUpdated(const TileMap *map)
{

}
