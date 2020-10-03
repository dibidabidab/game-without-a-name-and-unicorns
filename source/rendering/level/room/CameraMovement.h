
#ifndef GAME_CAMERAMOVEMENT_H
#define GAME_CAMERAMOVEMENT_H

#include <graphics/orthographic_camera.h>
#include "../../../tiled_room/TiledRoom.h"

class CameraMovement
{

    OrthographicCamera *cam;
    TiledRoom *room;

    float updateAccumulator = 0;

  public:

    vec2 offsetAnim = vec2(0);

    CameraMovement(TiledRoom *room, OrthographicCamera *cam) : room(room), cam(cam)
    {}

    void update(double deltaTime);

};


#endif
