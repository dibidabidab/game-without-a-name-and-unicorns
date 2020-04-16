
#include <gu/profiler.h>
#include "MultiplayerServerSession.h"
#include "../../level/ecs/components/Physics.h"
#include "../../level/ecs/components/PlatformerMovement.h"
#include "../../level/ecs/components/Networked.h"
#include "../../level/ecs/components/PlayerControlled.h"
#include "../../level/ecs/entity_templates/PlayerEntity.h"

using namespace Packet;
using namespace Packet::from_player;
using namespace Packet::from_server;

MultiplayerServerSession::MultiplayerServerSession(SocketServer *server) : server(server)
{
    server->onNewSocket = [&] (SharedSocket sock) mutable
    {
        Player_ptr player(new Player);
        player->id = ++playerIdCounter;
        assert(getPlayer(player->id) == NULL);

        playersJoiningAndLeaving.lock();
        playersJoining.push_back(player);
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

        io->addJsonPacketType<Level>();
        io->addJsonPacketType<entity_created>();
        io->addJsonPacketType<entity_destroyed>();
        io->addJsonPacketHandler<entity_data_update>([=](entity_data_update *packet) {

            networkingSystems.at(packet->roomI)->handleDataUpdate(packet, player.get());
            delete packet;
        });
        io->addJsonPacketHandler<entity_data_removed>([=](entity_data_removed *packet) {

            networkingSystems.at(packet->roomI)->handleDataRemoval(packet, player.get());
            delete packet;
        });

        io->addJsonPacketHandler<join_request>([=] (join_request *req) mutable {

            std::cout << req->name << " @" << player->io->socket->url << " attempting to join.\n";

            std::string declineReason;

            if (handleJoinRequest(player, req, declineReason))
            {
                std::cout << player->name << " @" << player->io->socket->url << " joined!\n";

                // playersJoiningAndLeaving.lock(); is already locked in MultiplayerServerSession::update
                deletePlayer(player->id, playersJoining);
                // playersJoiningAndLeaving.unlock();
            }
            else
            {
                join_request_declined p { declineReason };
                player->io->send(p);
            }
            delete req;
        });
    };

    server->start();
}


bool MultiplayerServerSession::handleJoinRequest(Player_ptr &player, join_request *req, std::string &declineReason)
{
    validateUsername(req->name, declineReason);

    if (!declineReason.empty())
        return false;

    player->name = req->name;

    player_joined msg { *player };
    sendPacketToAllPlayers(msg); // send to all players, except newly joined player

    players.push_back(player);

    if (player->io)
    {
        join_request_accepted acceptMsg { player->id };
        for (auto &p : players) acceptMsg.players.push_back(*p.get());

        player->io->send(acceptMsg);
    }
    else
    {
        assert(!localPlayer); // no support for split screen yet :P
        localPlayer = player;
    }
    if (level)
    {
        if (player->io)
            player->io->send(*level);
        createPlayerEntity(player);
    }
    return true;
}

void MultiplayerServerSession::update(double deltaTime)
{
    gu::profiler::Zone zone("server");

    playersJoiningAndLeaving.lock();
    for (auto it = playersJoining.rbegin(); it != playersJoining.rend(); ++it)
        (*it)->io->handlePackets();
    playersJoiningAndLeaving.unlock();

    {
        gu::profiler::Zone zone("packets in");
        for (auto &p : players) if (p->io)
        {
            gu::profiler::Zone zone(p->name);
            p->io->handlePackets();
        }
    }

    handleLeavingPlayers();

    if (level) level->update(deltaTime);
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
            removePlayerEntities(pId);
        }
    }
    playersLeaving.clear();
    playersJoiningAndLeaving.unlock();
}

void MultiplayerServerSession::setLevel(Level *newLevel)
{
    if (level && level->isUpdating())
        throw gu_err("cant set a level while updating");
    delete level;
    level = newLevel;
    addNetworkingSystemsToRooms();
    level->initialize();

    sendPacketToAllPlayers(*newLevel);
    onNewLevel(level);

    for (auto &p : players)
        createPlayerEntity(p);
}

void MultiplayerServerSession::createPlayerEntity(Player_ptr &player)
{
    Room &room = level->getRoom(0);
    auto e = room.getTemplate<PlayerEntity>()->createNetworked();
    room.entities.assign<PlayerControlled>(e, player->id);
}

void MultiplayerServerSession::removePlayerEntities(int playerId)
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

void MultiplayerServerSession::join(std::string username)
{
    std::cout << username << " attempting to join as local player.\n";

    Player_ptr p = std::make_shared<Player>();
    p->id = ++playerIdCounter;
    std::string declineReason;
    if (!handleJoinRequest(p, new join_request { username }, declineReason))
        onJoinRequestDeclined(declineReason);
}
