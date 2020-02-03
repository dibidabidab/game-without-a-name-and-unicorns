
#ifndef GAME_DEBUGTILERENDERER_H
#define GAME_DEBUGTILERENDERER_H


#include <graphics/3d/debug_line_renderer.h>
#include "Room.h"

namespace DebugTileRenderer
{

    void renderTile(DebugLineRenderer &lineRenderer, Tile tile, int x, int y, const vec3 &color)
    {
        switch (tile)
        {
            case Tile::empty: break;

            case Tile::full:
                lineRenderer.square(vec3(x, y, 0), 1, color);
                break;
            case Tile::slope_down:
                lineRenderer.line(vec3(x, y + 1, 0), vec3(x + 1, y, 0), color);
                lineRenderer.line(vec3(x, y, 0), vec3(x + 1, y, 0), color);
                lineRenderer.line(vec3(x, y, 0), vec3(x, y + 1, 0), color);
                break;
            case Tile::sloped_ceil_down:
                lineRenderer.line(vec3(x, y + 1, 0), vec3(x + 1, y, 0), color);
                lineRenderer.line(vec3(x, y + 1, 0), vec3(x + 1, y + 1, 0), color);
                lineRenderer.line(vec3(x + 1, y, 0), vec3(x + 1, y + 1, 0), color);
                break;
            case Tile::slope_up:
                lineRenderer.line(vec3(x, y, 0), vec3(x + 1, y + 1, 0), color);
                lineRenderer.line(vec3(x, y, 0), vec3(x + 1, y, 0), color);
                lineRenderer.line(vec3(x + 1, y, 0), vec3(x + 1, y + 1, 0), color);
                break;
            case Tile::sloped_ceil_up:
                lineRenderer.line(vec3(x, y, 0), vec3(x + 1, y + 1, 0), color);
                lineRenderer.line(vec3(x, y, 0), vec3(x, y + 1, 0), color);
                lineRenderer.line(vec3(x, y + 1, 0), vec3(x + 1, y + 1, 0), color);
                break;
            case Tile::platform:
                lineRenderer.line(vec3(x, y + 1, 0), vec3(x + 1, y + 1, 0), color);
                break;
        }
    }
};


#endif
