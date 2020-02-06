
#ifndef GAME_PLATFORMERMOVEMENT_H
#define GAME_PLATFORMERMOVEMENT_H


struct PlatformerMovement
{
    float jumpVelocity = 120;
    float walkVelocity = 50;

    bool jump = false, left = false, right = false, fall = false;

    // used by system:
    float fallPressedTimer = 0;
};

// todo: move this to other file?:
struct LocalPlayer {};

#endif
