
#ifndef GAME_ROOMEDITOR_H
#define GAME_ROOMEDITOR_H

#include <input/mouse_input.h>
#include <graphics/orthographic_camera.h>
#include <input/key_input.h>
#include "Room.h"
#include "DebugTileRenderer.h"

class RoomEditor
{

    Tile placing = Tile::full;

  public:

    void update(OrthographicCamera &cam, Room *room, DebugLineRenderer &lineRenderer)
    {
        ivec2 hoveredTile(cam.cursorTo2DWorldPos() / vec2(Level::PIXELS_PER_BLOCK));
        if (MouseInput::pressed(GLFW_MOUSE_BUTTON_LEFT))
            room->setTile(hoveredTile.x, hoveredTile.y, placing);

        DebugTileRenderer::renderTile(lineRenderer, placing, hoveredTile.x, hoveredTile.y, room->inRoom(hoveredTile.x, hoveredTile.y) ? mu::Y : mu::X);

        for (auto t : Room::TILE_TYPES)
            if (KeyInput::pressed(GLFW_KEY_0 + int(t)))
                placing = t;
    }
};


#endif
