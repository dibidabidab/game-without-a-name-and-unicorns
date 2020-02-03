
#ifndef GAME_PLATFORMERMOVEMENT_H
#define GAME_PLATFORMERMOVEMENT_H


struct PlatformerMovement
{
    float jumpVelocity = 100;
    float walkVelocity = 30;

    bool jump = false, left = false, right = false, fall = false;
};

// todo: move this to other file?:
struct LocalPlayer {};

#endif
