
#include "RoomOutliner.h"
#include "Room.h"

/*
 *  SOME VERY HORRIBLE CODE COPY-PASTED FROM A 2 YEAR OLD JAVA PROJECT
 *  the code works though
 */

void RoomOutliner::getOutlines(const Room *room, std::vector<std::pair<ivec2, ivec2>> &out)
{
    getWalls(room, out);
    getFloorsAndCeilings(room, out);
    getDownwardSlopes(room, out);
    getUpwardSlopes(room, out);
}

void RoomOutliner::getWalls(const Room *room, std::vector<std::pair<ivec2, ivec2>> &out)
{
    for (int x = 0; x < room->width; x++)
    {
        for (int i = 0; i < 2; i++)  // for left (0) and right (1) walls
        {
            int yBegin = -1;

            for (int y = 0; y < room->height; y++)
            {
                Tile air = room->getTile(x + (i == 0 ? 1 : -1), y);

                bool wall = room->getTile(x, y) == Tile::full &&
                            (air == Tile::empty || air == Tile::platform
                             || air == (i == 0 ? Tile::slope_up : Tile::slope_down)
                             || air == (i == 0 ? Tile::sloped_ceil_down : Tile::sloped_ceil_up));

                if (yBegin == -1 && wall) yBegin = y;
                else if (yBegin != -1 && !wall)
                {
                    int lineX = i == 0 ? x + 1 : x;

                    out.emplace_back(ivec2(lineX, yBegin), ivec2(lineX, y));
                    yBegin = -1;
                }
            }
        }
    }
}

void RoomOutliner::getFloorsAndCeilings(const Room *room, std::vector<std::pair<ivec2, ivec2>> &out)
{
    for (int y = 0; y < room->height; y++)
    {
        for (int i = 0; i < 2; i++)  // floor (0), ceiling (1)
        {
            int xBegin = -1;

            for (int x = 0; x < room->width; x++)
            {
                Tile air = room->getTile(x, y + (i == 0 ? 1 : -1));

                bool wall = room->getTile(x, y) == Tile::full &&
                            (air == Tile::empty || air == Tile::platform);

                if (xBegin == -1 && wall) xBegin = x;
                else if (xBegin != -1 && !wall)
                {
                    int lineY = i == 0 ? y + 1 : y;
                    out.emplace_back(ivec2(xBegin, lineY), ivec2(x, lineY));

                    xBegin = -1;
                }
            }
        }
    }
}

void RoomOutliner::getUpwardSlopes(const Room *room, std::vector<std::pair<ivec2, ivec2>> &out)
{
    for (int startY = -room->width; startY < room->height + room->width; startY++)
    {
        int start = -1;
        for (int x = 0; x < room->width; x++)
        {
            int y = startY + x;

            bool wall = room->getTile(x, y) == Tile::slope_up || room->getTile(x, y) == Tile::sloped_ceil_up;

            if (start != -1 && !wall) // end
            {
                out.emplace_back(ivec2(start, startY + start), ivec2(x, y));
                start = -1;
            }
            if (start == -1 && wall) start = x;
        }
    }
}

void RoomOutliner::getDownwardSlopes(const Room *room, std::vector<std::pair<ivec2, ivec2>> &out)
{
    for (int startY = 0; startY < room->height + room->width; startY++)
    {
        int start = -1;
        for (int x = 0; x < room->width; x++)
        {
            int y = startY - x;

            bool wall = room->getTile(x, y) == Tile::slope_down || room->getTile(x, y) == Tile::sloped_ceil_down;

            if (start != -1 && !wall) // end
            {
                out.emplace_back(ivec2(start, startY - start + 1), ivec2(x, y + 1));
                start = -1;
            }
            if (start == -1 && wall) start = x;
        }
    }
}

