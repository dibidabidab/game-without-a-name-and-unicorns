
#include <gu/profiler.h>
#include "MultiplayerServerSession.h"
#include "../../level/ecs/components/Physics.h"

using namespace Packet::from_player;
using namespace Packet::from_server;

MultiplayerServerSession::MultiplayerServerSession(SocketServer *server) : server(server)
{
    server->onNewSocket = [&] (SharedSocket sock) mutable
    {
        auto *player = new Player;
        player->id = ++playerIdCounter;
        assert(getPlayer(player->id) == NULL);

        playersJoiningAndLeaving.lock();
        playersJoining.push_back(Player_ptr(player));
        playersJoiningAndLeaving.unlock();

        auto io = player->io = new MultiplayerIO(sock);

        sock->onClose = [&, player]
        {
            // delete player
            // player deletes io
            // io deletes shared_ptr to socket
            // socket gets deleted

            playersJoiningAndLeaving.lock();
            playersLeaving.push_back(player->id);
            playersJoiningAndLeaving.unlock();
        };

        // ------ these handlers get executed in the game-loop: --------

        io->addJsonPacketType<join_request_accepted>();
        io->addJsonPacketType<join_request_declined>();
        io->addJsonPacketType<player_joined>();
        io->addJsonPacketType<player_left>();

        io->addJsonPacketHandler<join_request>([=](join_request *req){

            handleJoinRequest(player, req);
        });
    };

    server->start();
}


void MultiplayerServerSession::handleJoinRequest(Player *player, join_request *req)
{
    std::cout << req->name << " @" << player->io->socket->url << " attempting to join.\n";

    std::string declineReason;
    validateUsername(req->name, declineReason);

    if (!declineReason.empty())
    {
        join_request_declined p { declineReason };
        player->io->send(p);
        delete req;
        return;
    }

    player->name = req->name;

    for (int i = 0; i < playersJoining.size(); i++)
    {
        if (playersJoining[i].get() != player)
            continue;

        std::cout << player->name << " @" << player->io->socket->url << " joined!\n";

        player_joined msg { *player };
        sendPacketToAllPlayers(msg); // send to all players, except newly joined player

        players.push_back(playersJoining[i]);

//        playersJoiningAndLeaving.lock(); is already locked in MultiplayerServerSession::update
        playersJoining[i] = playersJoining.back();
        playersJoining.pop_back();
//        playersJoiningAndLeaving.unlock();

        join_request_accepted acceptMsg { player->id };
        for (auto &p : players) acceptMsg.players.push_back(*p.get());
        player->io->send(acceptMsg);

        break;
    }
    delete req;
}

void MultiplayerServerSession::update(double deltaTime)
{
    gu::profiler::Zone zone("server");

    playersJoiningAndLeaving.lock();
    for (int i = playersJoining.size() - 1; i >= 0; i--) // weird loop because handleJoinRequest() might delete item
        playersJoining[i]->io->handlePackets();
    playersJoiningAndLeaving.unlock();

    {
        gu::profiler::Zone zone("packets in");
        for (auto &p : players)
        {
            gu::profiler::Zone zone(p->name);
            p->io->handlePackets();
        }
    }

    handleLeavingPlayers();
}

void MultiplayerServerSession::handleLeavingPlayers()
{
    playersJoiningAndLeaving.lock();
    for (int pId : playersLeaving)
    {
        if (getPlayer(pId) == NULL)
        {
            // this means that the player didn't join before leaving
            auto deleted = deletePlayer(pId, playersJoining);
            std::cout << deleted->io->socket->url << " left without joining\n";
        }
        else
        {
            auto deleted = deletePlayer(pId, players);
            std::cout << deleted->name << " @" << deleted->io->socket->url << " left\n";
            player_left msg { pId };
            sendPacketToAllPlayers(msg);
        }
    }
    playersLeaving.clear();
    playersJoiningAndLeaving.unlock();
}
