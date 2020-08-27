
#include "TransRoomerSystem.h"
#include "../components/physics/Physics.h"
#include "../components/PlayerControlled.h"
#include "../components/TransRoomable.h"
#include "../../room/Room.h"

void TransRoomerSystem::update(double deltaTime, Room *room)
{
    int maxY = room->getMap().height * TileMap::PIXELS_PER_TILE - 1;
    int maxX = room->getMap().width * TileMap::PIXELS_PER_TILE - 1;

    room->entities.view<AABB, TransRoomable>().each([&](auto e, AABB &aabb, TransRoomable &transable) {

        ivec2 travelDir(0);

        if (aabb.center.x - aabb.halfSize.x <= 0)
            travelDir.x = -aabb.halfSize.x * 2 - 2;
        else if (aabb.center.x + aabb.halfSize.x >= maxX)
            travelDir.x = aabb.halfSize.x * 2 + 2;
        else if (aabb.center.y - aabb.halfSize.y <= 0)
            travelDir.y = -aabb.halfSize.y * 2 - 2;
        else if (aabb.center.y + aabb.halfSize.y >= maxY)
            travelDir.y = aabb.halfSize.y * 2 + 2;
        else
            return;

        ivec2 positionInNextRoom;

        Room *nextRoom = findNextRoom(*room, travelDir, aabb.center, positionInNextRoom);
        if (!nextRoom) // no room found
            return;

        std::cout << "TransRooming entity #" << int(e) << "! :D\n";

        try
        {
            entt::entity newEntity = nextRoom->entities.create();

            TransRoomed &transRoomed = nextRoom->entities.assign<TransRoomed>(newEntity);
            transRoomed.positionInNewRoom = positionInNextRoom;

            for (auto &compName : transable.archiveComponents)
            {
                auto utils = ComponentUtils::getFor(compName);
                if (!utils)
                    throw gu_err("Component " + compName + " does not exist!");

                if (!utils->entityHasComponent(e, room->entities))
                    continue;

                utils->getJsonComponentWithKeys(transRoomed.archivedComponents[compName], e, room->entities);
            }

            nextRoom->getTemplate(transable.templateName).createComponents(newEntity);

            {
                PlayerControlled *playerControlled = room->entities.try_get<PlayerControlled>(e);
                if (playerControlled)
                    nextRoom->entities.assign<PlayerControlled>(newEntity, *playerControlled);
                if (room->entities.has<LocalPlayer>(e))
                    nextRoom->entities.assign<LocalPlayer>(newEntity);
            }

            room->entities.destroy(e);

        } catch (std::exception &exc)
        {
            throw gu_err("Error while TransRooming entity#" + std::to_string(int(e)) + " (Template: " + transable.templateName + "):\n" + exc.what());
        }
    });
}

Room *TransRoomerSystem::findNextRoom(Room &current, const ivec2 &travelDir, const ivec2 &position, ivec2 &positionInNextRoom)
{
    ivec2 globalPos = position + ivec2(current.positionInLevel * uvec2(TileMap::PIXELS_PER_TILE)) + travelDir;

    for (int i = 0; i < current.getLevel().getNrOfRooms(); i++)
    {
        if (i == current.getIndexInLevel())
            continue;

        Room &r = current.getLevel().getRoom(i);

        ivec2
            roomPosMin = r.positionInLevel * uvec2(TileMap::PIXELS_PER_TILE),

            roomPosMax = roomPosMin + ivec2(r.getMap().width * TileMap::PIXELS_PER_TILE, r.getMap().height * TileMap::PIXELS_PER_TILE);

        if (globalPos.x >= roomPosMin.x && globalPos.x <= roomPosMax.x && globalPos.y >= roomPosMin.y && globalPos.y <= roomPosMax.y)
        {
            positionInNextRoom = globalPos - roomPosMin;
            return &r;
        }
    }
    return NULL;
}
