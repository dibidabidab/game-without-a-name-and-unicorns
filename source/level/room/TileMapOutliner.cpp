
#include "TileMapOutliner.h"
#include "TileMap.h"

/*
 *  SOME VERY HORRIBLE CODE COPY-PASTED FROM A 2 YEAR OLD JAVA PROJECT
 *  the code works though
 */

void TileMapOutliner::getOutlines(const TileMap *map, std::vector<std::pair<vec2, vec2>> &out)
{
    getWalls(map, out);
    getFloorsAndCeilings(map, out);
    getDownwardSlopes(map, out);
    getUpwardSlopes(map, out);
    getDownwardHalfSlopes(map, out);
    getUpwardHalfSlopes(map, out);
}

void TileMapOutliner::getWalls(const TileMap *map, std::vector<std::pair<vec2, vec2>> &out)
{
    for (int x = 0; x < map->width; x++)
    {
        for (int i = 0; i < 2; i++)  // for left (0) and right (1) walls
        {
            int yBegin = -1;

            for (int y = 0; y <= map->height; y++)
            {
                Tile air = map->getTile(x + (i == 0 ? 1 : -1), y);

                bool wall = map->getTile(x, y) == Tile::full &&
                            (air == Tile::empty || air == Tile::platform
                             || air == (i == 0 ? Tile::slope_up : Tile::slope_down)
                             || air == (i == 0 ? Tile::sloped_ceil_down : Tile::sloped_ceil_up));

                if (yBegin == -1 && wall) yBegin = y;
                else if (yBegin != -1 && !wall)
                {
                    int lineX = i == 0 ? x + 1 : x;

                    if (i == 1)
                        out.emplace_back(vec2(lineX, yBegin), vec2(lineX, y));
                    else
                        out.emplace_back(vec2(lineX, y), vec2(lineX, yBegin));
                    yBegin = -1;
                }
            }
        }
    }
}

void TileMapOutliner::getFloorsAndCeilings(const TileMap *map, std::vector<std::pair<vec2, vec2>> &out)
{
    for (int y = 0; y < map->height; y++)
    {
        for (int i = 0; i < 2; i++)  // floor (0), ceiling (1)
        {
            int xBegin = -1;

            for (int x = 0; x <= map->width; x++)
            {
                Tile air = map->getTile(x, y + (i == 0 ? 1 : -1));

                bool wall = map->getTile(x, y) == Tile::full &&
                            (air == Tile::empty || air == Tile::platform);

                if (xBegin == -1 && wall) xBegin = x;
                else if (xBegin != -1 && !wall)
                {
                    int lineY = i == 0 ? y + 1 : y;
                    if (i == 1)
                        out.emplace_back(vec2(x, lineY), vec2(xBegin, lineY));
                    else
                        out.emplace_back(vec2(xBegin, lineY), vec2(x, lineY));

                    xBegin = -1;
                }
            }
        }
    }
}

void TileMapOutliner::getUpwardSlopes(const TileMap *map, std::vector<std::pair<vec2, vec2>> &out)
{
    for (int startY = -map->width; startY < map->height + map->width; startY++)
    {
        int start = -1;
        Tile startTile;
        for (int x = 0; x <= map->width; x++)
        {
            int y = startY + x;

            Tile t = map->getTile(x, y);
            bool wall = t == Tile::slope_up || t == Tile::sloped_ceil_up;

            if (start != -1 && !wall) // end
            {
                if (startTile == Tile::slope_up)
                    out.emplace_back(vec2(start, startY + start), vec2(x, y));
                else
                    out.emplace_back(vec2(x, y), vec2(start, startY + start));
                start = -1;
            }
            if (start == -1 && wall)
            {
                start = x;
                startTile = t;
            }
        }
    }
}

void TileMapOutliner::getDownwardSlopes(const TileMap *map, std::vector<std::pair<vec2, vec2>> &out)
{
    for (int startY = 0; startY < map->height + map->width; startY++)
    {
        int start = -1;
        Tile startTile;
        for (int x = 0; x <= map->width; x++)
        {
            int y = startY - x;

            Tile t = map->getTile(x, y);
            bool wall = t == Tile::slope_down || t == Tile::sloped_ceil_down;

            if (start != -1 && !wall) // end
            {
                if (startTile == Tile::slope_down)
                    out.emplace_back(vec2(start, startY - start + 1), vec2(x, y + 1));
                else
                    out.emplace_back(vec2(x, y + 1), vec2(start, startY - start + 1));
                start = -1;
            }
            if (start == -1 && wall)
            {
                start = x;
                startTile = t;
            }
        }
    }
}

void TileMapOutliner::getDownwardHalfSlopes(const TileMap *map, std::vector<std::pair<vec2, vec2>> &out)
{
    for (int x = 0; x <= map->width; x++)
        for (int y = 0; y <= map->height; y++)
            if (map->getTile(x, y) == Tile::slope_down_half0)
                out.emplace_back(vec2(x, y + 1), vec2(x + 1, y + .5));
            else if (map->getTile(x, y) == Tile::slope_down_half1)
                out.emplace_back(vec2(x, y + .5), vec2(x + 1, y));
}

void TileMapOutliner::getUpwardHalfSlopes(const TileMap *map, std::vector<std::pair<vec2, vec2>> &out)
{
    for (int x = 0; x <= map->width; x++)
        for (int y = 0; y <= map->height; y++)
            if (map->getTile(x, y) == Tile::slope_up_half0)
                out.emplace_back(vec2(x, y), vec2(x + 1, y + .5));
            else if (map->getTile(x, y) == Tile::slope_up_half1)
                out.emplace_back(vec2(x, y + .5), vec2(x + 1, y + 1));
}

