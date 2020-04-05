#include <memory>

#include <utility>
#include <gu/profiler.h>


#include "MultiplayerClientSession.h"
#include "../../level/ecs/components/Physics.h"

using namespace Packet::from_player;
using namespace Packet::from_server;

MultiplayerClientSession::MultiplayerClientSession(SharedSocket socket) : io(socket)
{
    io.addJsonPacketType<join_request>();

    io.addJsonPacketHandler<join_request_declined>([&](join_request_declined *declined) {
        std::cout << "Join request declined. Reason: " << declined->reason << "\n";
        onJoinRequestDeclined(declined->reason);
        delete declined;
    });

    io.addJsonPacketHandler<join_request_accepted>([&](join_request_accepted *accepted) {
        std::cout << "Join request accepted. Your player ID: " << accepted->yourPlayerId << "\n";

        for (Player &p : accepted->players)
        {
            players.push_back(std::make_shared<Player>(p));
            if (p.id == accepted->yourPlayerId)
                localPlayer = players.back();
        }
        delete accepted;
    });

    io.addJsonPacketHandler<player_joined>([&](auto *joined) {

        std::cout << joined->player.name << " joined the game!\n";
        players.push_back(std::make_shared<Player>(joined->player));
        delete joined;
    });
    io.addJsonPacketHandler<player_left>([&](auto *left) {

        std::cout << deletePlayer(left->playerId)->name << " left the game\n";
        delete left;
    });

    socket->onClose = [&]
    {
        std::cout << "k bye\n";
    };
}

void MultiplayerClientSession::update(double deltaTime)
{
    gu::profiler::Zone zone("multiplayer client");
    {
        gu::profiler::Zone zone("packets in");
        io.handlePackets();
    }
}

void MultiplayerClientSession::join(std::string username)
{
    join_request req { std::move(username) };
    io.send(req);
}
