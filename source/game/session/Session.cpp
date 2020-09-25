
#include "Session.h"
#include "../../generated/PlayerControlled.hpp"

const Player_ptr &Session::getPlayerById(int id) const
{
    for (auto &p : players) if (p->id == id) return p;
    throw gu_err("Player " + std::to_string(id) + " does not exist!");
}

void Session::validateUsername(const std::string &name, std::string &declineReason) const
{
    for (const char &c : name)
        if (c < ' ' || c > '~') // http://www.asciitable.com/
            declineReason = "Please use normal characters in your name.\n";

    if (name.empty() || name.size() > 32)
        declineReason = "Username length must be 1-32.";

    for (auto &p : players)
        if (p->name == name)
            declineReason = "Username already taken. :(";
}

Player_ptr Session::deletePlayer(int pId, std::list<Player_ptr> &players)
{
    Player_ptr deletedPlayer;
    players.remove_if([&](auto &p) {
        if (pId == p->id)
        {
            deletedPlayer = p;
            return true;
        }
        return false;
    });
    if (!deletedPlayer) throw gu_err("Player not found");
    return deletedPlayer;
}

Player_ptr Session::getPlayer(int id) const
{
    for (auto &p : players) if (p->id == id) return p;
    return NULL;
}

Session::Session(const char *saveGamePath) : saveGame(saveGamePath)
{

}

void Session::spawnPlayerEntities(bool networked)
{
    assert(level != NULL);
    for (auto &player : players)
        spawnPlayerEntity(player, networked);
}

void Session::spawnPlayerEntity(Player_ptr &p, bool networked)
{
    if (level->getNrOfRooms() >= 1)
    {
        Room *spawnRoom = level->getRoomByName(level->spawnRoom.c_str());
        if (!spawnRoom)
            spawnRoom = &level->getRoom(0);

        auto &templ = spawnRoom->getTemplate("Player");
        auto e = networked ? templ.createNetworked() : templ.create();
        PlayerControlled pc;
        pc.playerId = p->id;
        spawnRoom->entities.assign<PlayerControlled>(e, pc);
        if (p == localPlayer)
            spawnRoom->entities.assign<LocalPlayer>(e);
    }
    else throw gu_err("Cant spawn Player entity in an empty Level!");
}


void Session::removePlayerEntities(int playerId)
{
    for (int i = 0; i < level->getNrOfRooms(); i++)
    {
        Room &r = level->getRoom(i);
        r.entities.view<PlayerControlled>().each([&] (entt::entity e, PlayerControlled &pC) {
            if (pC.playerId == playerId)
                r.entities.destroy(e);
        });
    }
}

Session::~Session()
{
    delete level;
    saveGame.save();
}
