
#include "TileMapRenderer.h"

#include <graphics/3d/mesh.h>
#include <gu/profiler.h>

TileMapRenderer::TileMapRenderer(TileMap *map)
    : map(map),
      tileShader(
          "tileShader",
          "shaders/tile_map/tile.vert",
          "shaders/tile_map/tile.frag"
      ),
      fbo(
          map->width * TileMap::PIXELS_PER_TILE,
          map->height * TileMap::PIXELS_PER_TILE
      ),
      mapShader(
          "tileMapShader",
          "shaders/tile_map/tile_map.vert",
          "shaders/tile_map/tile_map.frag"
      ),
      lastTileSetReloadTime(glfwGetTime())
{
    fbo.addColorTexture(GL_R8UI, GL_RED_INTEGER, GL_NEAREST, GL_NEAREST);
    fbo.addDepthBuffer();
}

void TileMapRenderer::createMapTexture()
{
    fbo.bind();
    glEnable(GL_DEPTH_TEST);

    uint zero = 0;
    glClearBufferuiv(GL_COLOR, 0, &zero);
    glClear(GL_DEPTH_BUFFER_BIT);

    tileShader.use();
    glUniform2i(tileShader.location("mapSize"), map->width, map->height);

    for (int x = 0; x < map->width; x++)
        for (int y = 0; y < map->height; y++)
            renderTile(x, y);

    glDisable(GL_DEPTH_TEST);
    fbo.unbind();
    textureCreated = true;
}

void TileMapRenderer::renderTile(int x, int y)
{
    TileMaterial material = map->getMaterial(x, y);

    auto &tileSet = map->getMaterialProperties(material).tileSet;

    auto subTexture = tileSet->getSubTextureForTile(*map, x, y);
    if (!subTexture)
        return;

    glUniform2i(tileShader.location("tilePos"), x, y);
    glUniform2i(tileShader.location("tileTextureOffset"), subTexture->offset.x, subTexture->offset.y);

    int variation = mu::randomIntFromX(x + x * y + map->width + map->height, tileSet->variations.size());

    tileSet->variations.at(variation)->bind(0, tileShader, "tileSet");

    Mesh::getQuad()->render();
}

void TileMapRenderer::updateMapTextureIfNeeded()
{
    gu::profiler::Zone z("tilemap render");

    bool tileSetReloaded = false;

    for (int i = 0; i < map->nrOfMaterialTypes; i++)
    {
        if (map->getMaterialProperties(i).tileSet.getLoadedAsset().loadedSinceTime > lastTileSetReloadTime)
        {
            tileSetReloaded = true;
            lastTileSetReloadTime = glfwGetTime();
        }
    }

    if (!map->updatesPrevUpdate().empty() || !textureCreated || tileSetReloaded)
        createMapTexture();
}

void TileMapRenderer::render(const Camera &cam, const SharedTexture &bloodSplatterTexture)
{
    gu::profiler::Zone z("tilemap render");

    mapShader.use();

    glUniform2f(mapShader.location("camPos"), cam.position.x / float(fbo.width), cam.position.y / float(fbo.height));
    glUniform2f(mapShader.location("camSize"), cam.viewportWidth / float(fbo.width), cam.viewportHeight / float(fbo.height));
    fbo.colorTexture->bind(0, mapShader, "mapTexture");
    bloodSplatterTexture->bind(1, mapShader, "bloodSplatterTexture");

    Mesh::getQuad()->render();
}
