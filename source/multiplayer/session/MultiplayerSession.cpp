#include <memory>


#include "MultiplayerSession.h"
#include "../packets.h"

void MultiplayerSession::validateUsername(const std::string &name, std::string &declineReason) const
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

Player_ptr MultiplayerSession::deletePlayer(int pId, std::vector<Player_ptr> &players)
{
    for (int i = 0; i < players.size(); i++)
    {
        Player_ptr p = players[i];
        if (p->id != pId) continue;

        players.erase(players.begin() + i);
        return p;
    }
    throw gu_err("Player not found");
}

Player_ptr MultiplayerSession::getPlayer(int id)
{
    for (auto &p : players) if (p->id == id) return p;
    return NULL;
}
