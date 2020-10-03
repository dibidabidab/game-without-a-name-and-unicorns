
#ifndef GAME_DEBUGTILERENDERER_H
#define GAME_DEBUGTILERENDERER_H


#include <graphics/3d/debug_line_renderer.h>
#include "../../../tiled_room/TileMap.h"

namespace DebugTileRenderer
{

    inline void renderTile(DebugLineRenderer &lineRenderer, Tile tile, int x, int y, const vec3 &color)
    {
        switch (tile)
        {
            case Tile::empty: break;

            case Tile::full:
                lineRenderer.square(vec3(x, y, 0), 1, color);
                break;
            case Tile::slope_down:
                lineRenderer.line(vec2(x, y + 1), vec2(x + 1, y), color);
                lineRenderer.line(vec2(x, y), vec2(x + 1, y), color);
                lineRenderer.line(vec2(x, y), vec2(x, y + 1), color);
                break;
            case Tile::sloped_ceil_down:
                lineRenderer.line(vec2(x, y + 1), vec2(x + 1, y), color);
                lineRenderer.line(vec2(x, y + 1), vec2(x + 1, y + 1), color);
                lineRenderer.line(vec2(x + 1, y), vec2(x + 1, y + 1), color);
                break;
            case Tile::slope_up:
                lineRenderer.line(vec2(x, y), vec2(x + 1, y + 1), color);
                lineRenderer.line(vec2(x, y), vec2(x + 1, y), color);
                lineRenderer.line(vec2(x + 1, y), vec2(x + 1, y + 1), color);
                break;
            case Tile::sloped_ceil_up:
                lineRenderer.line(vec2(x, y), vec2(x + 1, y + 1), color);
                lineRenderer.line(vec2(x, y), vec2(x, y + 1), color);
                lineRenderer.line(vec2(x, y + 1), vec2(x + 1, y + 1), color);
                break;
            case Tile::platform:
                lineRenderer.line(vec2(x, y + 1), vec2(x + 1, y + 1), color);
                break;
            case Tile::slope_down_half0:
                lineRenderer.line(vec2(x, y + 1), vec2(x + 1, y + .5), color);
                lineRenderer.line(vec2(x, y), vec2(x + 1, y), color);
                lineRenderer.line(vec2(x, y), vec2(x, y + 1), color);
                break;
            case Tile::slope_down_half1:
                lineRenderer.line(vec2(x, y + .5), vec2(x + 1, y), color);
                lineRenderer.line(vec2(x, y), vec2(x + 1, y), color);
                break;
            case Tile::slope_up_half0:
                lineRenderer.line(vec2(x, y), vec2(x + 1, y + .5), color);
                lineRenderer.line(vec2(x, y), vec2(x + 1, y), color);
                break;
            case Tile::slope_up_half1:
                lineRenderer.line(vec2(x, y + .5), vec2(x + 1, y + 1), color);
                lineRenderer.line(vec2(x, y), vec2(x + 1, y), color);
                lineRenderer.line(vec2(x + 1, y), vec2(x + 1, y + 1), color);
                break;
        }
    }
};


#endif
