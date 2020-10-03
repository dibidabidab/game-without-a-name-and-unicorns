
#include "WindMap.h"
#include "TileMap.h"

WindMap::WindMap(const ivec2 &size) : mapSize(size)
{
    vectors = new vec2[size.x * size.y];
    std::fill_n(vectors, size.x * size.y, vec2(0));
}

WindMap::~WindMap()
{
    delete[] vectors;
}

vec2 &WindMap::get(uint8 x, uint8 y)
{
    if (x >= mapSize.x)
        x = mapSize.x - 1;
    if (y >= mapSize.y)
        y = mapSize.y - 1;
    return vectors[x * mapSize.y + y];
}

const vec2 &WindMap::get(uint8 x, uint8 y) const
{
    return ((WindMap *) this)->get(x, y);
}

vec2 &WindMap::getAtPixelCoordinates(int x, int y)
{
    return get(x / TileMap::PIXELS_PER_TILE, y / TileMap::PIXELS_PER_TILE);
}

const vec2 &WindMap::getAtPixelCoordinates(int x, int y) const
{
    return ((WindMap *) this)->getAtPixelCoordinates(x, y);
}

void WindMap::update(float deltaTime)
{
    for (int x = 0; x < mapSize.x; x++)
    {
        for (int y = 0; y < mapSize.y; y++)
        {
            vec2 surrounding = get(x - 1, y) + get(x + 1, y) + get(x, y - 1) + get(x, y + 1);
            surrounding *= .25;

            constexpr float SPREAD = 200;
            constexpr float DECREASE = 2;

            vec2 &windDirection = get(x, y);

            float spread = min(1.f, SPREAD * deltaTime);
            float decrease = min(1.f, DECREASE * deltaTime);

            windDirection *= 1. - spread;
            windDirection += surrounding * spread;
            windDirection *= 1. - decrease;
        }
    }
}
