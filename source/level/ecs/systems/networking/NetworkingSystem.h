
#ifndef GAME_NETWORKINGSYSTEM_H
#define GAME_NETWORKINGSYSTEM_H


#include "../EntitySystem.h"
#include "../../../Level.h"
#include "../../components/Networked.h"
#include "../../../../multiplayer/session/MultiplayerSession.h"
#include "../../components/PlayerControlled.h"

class NetworkingSystem : public EntitySystem
{
    MultiplayerSession *mpSession;
    Room *room;
    std::list<Player_ptr> playersInRoom;

  public:
    NetworkingSystem(MultiplayerSession *mpSession) : EntitySystem("network"), mpSession(mpSession)
    {

    }

  protected:
    void init(Room *room_) override
    {
        room = room_;
        room->entities.on_construct<Networked>().connect<&NetworkingSystem::onNetworkedEntityCreated>(this);
        room->entities.on_destroy<Networked>().connect<&NetworkingSystem::onNetworkedEntityDestroyed>(this);

        room->entities.on_construct<PlayerControlled>().connect<&NetworkingSystem::onPlayerEnteredRoom>(this);
        room->entities.on_destroy<PlayerControlled>().connect<&NetworkingSystem::onPlayerLeftRoom>(this);
    }

    void onNetworkedEntityCreated(entt::registry &reg, entt::entity entity)
    {
        Networked &networked = reg.get<Networked>(entity);
        std::cout << "Entity " << int(entity) << ":" << networked.networkID << " created\n";

        if (!mpSession->isServer())
            return;
        // send to players.
        bool sentOnce = false;
        for (auto &p : playersInRoom)
            if (!sentOnce)
                sentOnce = sendCreatedEntity(networked, p);
            else
                mpSession->resendPacketToAnotherPlayer(p);
    }

    bool sendCreatedEntity(Networked &networked, const Player_ptr &player)
    {
        Packet::from_server::entity_created packet {
            room->getIndexInLevel(),
            networked.networkID
        };
        return mpSession->sendPacketToPlayer(packet, player);
    }

    void onPlayerEnteredRoom(entt::registry &reg, entt::entity entity)
    {
        PlayerControlled &pC = reg.get<PlayerControlled>(entity);

        Player_ptr player = mpSession->getPlayerById(pC.playerId);
        playersInRoom.push_back(player);

        if (!player->io) return;

        std::cout << "Sending all entities in room " << room->getIndexInLevel() << " to " << player->name << "\n";

        room->entities.view<Networked>().each([&](auto e, Networked &networked) {

            sendCreatedEntity(networked, player);

            if (!networked.toSend) return;

            for (auto *c : networked.toSend->list)
            {
                if (networked.dataPresence[c->getDataTypeHash()] == false)
                    continue;

                json jsonToSend;
                c->dataToJson(jsonToSend, e, room->entities);
                sendEntityDataUpdate(networked, jsonToSend, c, player);
            }
        });
    }

    void onPlayerLeftRoom(entt::registry &reg, entt::entity entity)
    {
        PlayerControlled &pC = reg.get<PlayerControlled>(entity);
        int sizeBefore = playersInRoom.size();
        playersInRoom.remove_if([&](auto &p) { return pC.playerId == p->id; });
        assert(sizeBefore == playersInRoom.size() - 1);
    }

    void onNetworkedEntityDestroyed(entt::registry &reg, entt::entity entity)
    {
        Networked &networked = reg.get<Networked>(entity);
        std::cout << "Entity " << int(entity) << ":" << networked.networkID << " destroyed\n";
    }

    void update(double deltaTime, Room *room) override
    {
        room->entities.view<Networked>().each([&](auto e, Networked &networked) {

            if (!networked.toSend) return;

            for (auto *c : networked.toSend->list)
            {
                gu::profiler::Zone z(c->getDataTypeName());
                json jsonToSend;
                bool hasChanged = false, isPresent = true;
                c->dataToJsonIfChanged(hasChanged, isPresent, jsonToSend, e, room->entities);

                bool wasPresent = networked.dataPresence[c->getDataTypeHash()];

                if (wasPresent && !isPresent)
                {
                    networked.dataPresence[c->getDataTypeHash()] = false;
                    std::cout << c->getDataTypeName() << " was deleted from " << int(e) << ":" << networked.networkID <<"\n";
                }

                if (hasChanged && isPresent)
                {
                    std::cout << c->getDataTypeName() << " changed for " << int(e) << ":" << networked.networkID << ":\n";// << jsonToSend << "\n";

                    bool sentOnce = false;
                    for (auto &p : playersInRoom)
                        if (!sentOnce)
                            sentOnce = sendEntityDataUpdate(networked, jsonToSend, c, p);
                        else
                            mpSession->resendPacketToAnotherPlayer(p);
                }
            }
        });
    }

    bool sendEntityDataUpdate(Networked &networked, json &data, AbstractNetworkedData *dataType, const Player_ptr &sendTo)
    {
        Packet::entity_data_update packet;
        packet.roomI = room->getIndexInLevel();
        packet.dataTypeHash = dataType->getDataTypeHash();
        packet.entityNetworkId = networked.networkID;
        packet.jsonData = data;
        return mpSession->sendPacketToPlayer(packet, sendTo);
    }

};


#endif
