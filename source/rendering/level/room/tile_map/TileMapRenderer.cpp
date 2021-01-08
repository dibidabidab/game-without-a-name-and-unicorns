
#include "TileMapRenderer.h"

#include "../../../../tiled_room/TiledRoom.h"
#include "../../../../tiled_room/TileMap.h"

#include <graphics/3d/mesh.h>
#include <gu/profiler.h>

TileMapRenderer::TileMapRenderer(const TiledRoom &r)
    : room(r),
      tileShader(
          "tileShader",
          "shaders/tile_map/tile.vert",
          "shaders/tile_map/tile.frag"
      ),
      fbo(
          r.getMap().width * TileMap::PIXELS_PER_TILE,
          r.getMap().height * TileMap::PIXELS_PER_TILE
      ),
      mapShader(
          "tileMapShader",
          "shaders/tile_map/tile_map.vert",
          "shaders/tile_map/tile_map.frag"
      ),
      lastTileSetReloadTime(glfwGetTime())
{
    fbo.addColorTexture(GL_R8UI, GL_RED_INTEGER, GL_NEAREST, GL_NEAREST);
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
    glUniform2i(tileShader.location("mapSize"), room.getMap().width, room.getMap().height);

    for (int x = 0; x < room.getMap().width; x++)
        for (int y = 0; y < room.getMap().height; y++)
            {
                renderTile(room.getMap(), x, y);

                for (auto &m : room.decorativeTileLayers)
                    renderTile(m, x, y);
            }

    glDisable(GL_DEPTH_TEST);
    fbo.unbind();
    textureCreated = true;
}

void TileMapRenderer::renderTile(const TileMap &map, int x, int y)
{
    TileMaterial material = map.getMaterial(x, y);

    auto &tileSet = map.getMaterialProperties(material).tileSet;

    auto subTexture = tileSet->getSubTextureForTile(map, x, y);
    if (!subTexture)
        return;

    glUniform2i(tileShader.location("tilePos"), x, y);
    glUniform2i(tileShader.location("tileTextureOffset"), subTexture->offset.x, subTexture->offset.y);

    glUniform1f(tileShader.location("zIndex"), map.zIndex);

    int variation = mu::randomIntFromX(x + x * y + map.width + map.height, tileSet->variations.size());

    tileSet->variations.at(variation)->bind(0, tileShader, "tileSet");

    Mesh::getQuad()->render();
}

void TileMapRenderer::updateMapTextureIfNeeded()
{
    gu::profiler::Zone z("tilemap render");

    bool tileSetReloaded = false;

    auto &map = room.getMap();

    for (int i = 0; i < map.nrOfMaterialTypes; i++) // TODO: this will not work if different layers get different material/properties/types. Then looping through all layers will be required
    {
        if (map.getMaterialProperties(i).tileSet.getLoadedAsset().loadedSinceTime > lastTileSetReloadTime)
        {
            tileSetReloaded = true;
            lastTileSetReloadTime = glfwGetTime();
        }
    }

    bool zIndexChanged = false;
    prevZIndexes.resize(room.decorativeTileLayers.size() + 1, -1);
    if (map.zIndex != prevZIndexes[0])
    {
        zIndexChanged = true;
        prevZIndexes[0] = map.zIndex;
    }

    bool decChanged = false;
    int i = 0;
    for (auto &layer : room.decorativeTileLayers)
    {
        if (!layer.updatesPrevUpdate().empty())
            decChanged = true;
        if (layer.zIndex != prevZIndexes[++i])
        {
            zIndexChanged = true;
            prevZIndexes[i] = layer.zIndex;
        }
    }

    if (decChanged || zIndexChanged || !map.updatesPrevUpdate().empty() || !textureCreated || tileSetReloaded)
        createMapTexture();
}

void TileMapRenderer::render(const Camera &cam, const SharedTexture &bloodSplatterTexture)
{
    gu::profiler::Zone z("tilemap render");

    mapShader.use();

    glUniform2f(mapShader.location("camPos"), cam.position.x / float(fbo.width), cam.position.y / float(fbo.height));
    glUniform2f(mapShader.location("camSize"), cam.viewportWidth / float(fbo.width), cam.viewportHeight / float(fbo.height));
    fbo.colorTexture->bind(0, mapShader, "mapTexture");
    fbo.colorTextures.at(1)->bind(1, mapShader, "depth8BitTexture");
    bloodSplatterTexture->bind(2, mapShader, "bloodSplatterTexture");

    Mesh::getQuad()->render();
}
