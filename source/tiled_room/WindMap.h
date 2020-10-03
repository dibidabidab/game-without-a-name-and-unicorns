
#ifndef GAME_WINDMAP_H
#define GAME_WINDMAP_H

#include <utils/math_utils.h>

class WindMap
{

    vec2 *vectors;
    ivec2 mapSize;

  public:

    WindMap(const ivec2 &size);

    vec2 &get(uint8 x, uint8 y);

    const vec2 &get(uint8 x, uint8 y) const;

    vec2 &getAtPixelCoordinates(int x, int y);

    const vec2 &getAtPixelCoordinates(int x, int y) const;

    void update(float deltaTime);

    ~WindMap();

};


#endif
