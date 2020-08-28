
#ifndef GAME_ROOMEDITOR_H
#define GAME_ROOMEDITOR_H

#include <gl_includes.h>
#include <imgui.h>
#include <input/mouse_input.h>
#include <graphics/orthographic_camera.h>
#include <input/key_input.h>
#include "../../level/room/TileMap.h"
#include "DebugTileRenderer.h"

class RoomEditor
{

    Tile placing = Tile::full;

    TileMaterial material = 0;

  public:

    bool show = false;

    void update(OrthographicCamera &cam, TileMap *room, DebugLineRenderer &lineRenderer);
};


#endif
