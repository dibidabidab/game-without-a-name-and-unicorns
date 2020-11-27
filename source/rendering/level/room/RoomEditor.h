
#ifndef GAME_ROOMEDITOR_H
#define GAME_ROOMEDITOR_H

#include <gl_includes.h>
#include <imgui.h>
#include <input/mouse_input.h>
#include <graphics/orthographic_camera.h>
#include <input/key_input.h>
#include "../../../tiled_room/TileMap.h"
#include "DebugTileRenderer.h"

class TiledRoom;

class RoomEditor
{

    Tile placing = Tile::full;

    TileMaterial material = 0;

    int selectedLayer = 0;

  public:

    bool show = false;

    void update(OrthographicCamera &cam, TiledRoom &room, DebugLineRenderer &lineRenderer);
};


#endif
