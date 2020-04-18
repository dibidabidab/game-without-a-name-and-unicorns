
#ifndef GAME_CAMERAMOVEMENT_H
#define GAME_CAMERAMOVEMENT_H

#include <graphics/orthographic_camera.h>
#include "../../level/room/Room.h"

class CameraMovement
{

    OrthographicCamera *cam;
    Room *room;

    float updateAccumulator = 0;

  public:
    CameraMovement(Room *room, OrthographicCamera *cam) : room(room), cam(cam)
    {}

    void update(double deltaTime);

};


#endif
