
#include "Session.h"

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
