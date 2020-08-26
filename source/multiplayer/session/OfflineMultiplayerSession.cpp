
#include "OfflineMultiplayerSession.h"
#include "../../level/ecs/components/PlayerControlled.h"

void OfflineMultiplayerSession::join(std::string username)
{
    std::string declineReason;
    validateUsername(username, declineReason);

    if (!declineReason.empty())
    {
        onJoinRequestDeclined(declineReason);
        return;
    }
    assert(players.empty()); // splitscreen not yet supported

    localPlayer = std::make_shared<Player>();
    localPlayer->id = 69420;
    localPlayer->name = username;
    players.push_back(localPlayer);
}

void OfflineMultiplayerSession::update(double deltaTime)
{
    if (firstUpdate)
    {
        firstUpdate = false;
        level->initialize();
        onNewLevel(level);

        if (level->getNrOfRooms() >= 1)
        {
            Room &room = level->getRoom(0);
            auto e = room.getTemplate("Player").create();
            room.entities.assign<PlayerControlled>(e, localPlayer->id);
            room.entities.assign<LocalPlayer>(e);
        }
    }
    level->update(deltaTime);
}

OfflineMultiplayerSession::OfflineMultiplayerSession(Level *lvl) {
    assert(lvl != NULL);
    level = lvl;
}
