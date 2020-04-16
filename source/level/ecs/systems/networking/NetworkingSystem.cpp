#include "NetworkingSystem.h"
#include "../../components/Networked.h"
#include "../../components/PlayerControlled.h"
#include "../../../../multiplayer/session/MultiplayerSession.h"

void NetworkingSystem::init(Room *room_)
{
    room = room_;
    room->entities.on_construct<Networked>().connect<&NetworkingSystem::onNetworkedEntityCreated>(this);
    room->entities.on_destroy<Networked>().connect<&NetworkingSystem::onNetworkedEntityDestroyed>(this);

    room->entities.on_construct<PlayerControlled>().connect<&NetworkingSystem::onPlayerEnteredRoom>(this);
    room->entities.on_destroy<PlayerControlled>().connect<&NetworkingSystem::onPlayerLeftRoom>(this);
}

void NetworkingSystem::onNetworkedEntityCreated(entt::registry &reg, entt::entity entity)
{
    Networked &networked = reg.get<Networked>(entity);
    std::cout << "Entity " << int(entity) << ":" << networked.networkID << " created\n";
    assert(networkIdToEntity.find(networked.networkID) == networkIdToEntity.end());
    networkIdToEntity[networked.networkID] = entity;

    if (!mpSession->isServer())
        return;
    // send to players.
    auto p = entityCreatedPacket(networked);
    mpSession->sendPacketToPlayers(p, playersInRoom);
}

void NetworkingSystem::onPlayerEnteredRoom(entt::registry &reg, entt::entity entity)
{
    PlayerControlled &pC = reg.get<PlayerControlled>(entity);

    Player_ptr player = mpSession->getPlayerById(pC.playerId);
    playersInRoom.push_back(player);

    if (!player->io) return;

    std::cout << "Sending all entities in room " << room->getIndexInLevel() << " to " << player->name << "\n";

    room->entities.view<Networked>().each([&](auto e, Networked &networked) {

        auto p = entityCreatedPacket(networked);
        player->io->send(p);

        for (auto &c : networked.toSend.list)
        {
            if (networked.dataPresence[c->getDataTypeHash()] == false)
                continue;

            json jsonToSend;
            c->dataToJson(jsonToSend, e, room->entities);
            auto du = dataUpdatePacket(networked, jsonToSend, c);
            player->io->send(du);
        }
    });
}

void NetworkingSystem::onPlayerLeftRoom(entt::registry &reg, entt::entity entity)
{
    PlayerControlled &pC = reg.get<PlayerControlled>(entity);

    mpSession->deletePlayer(pC.playerId, playersInRoom);
}

void NetworkingSystem::onNetworkedEntityDestroyed(entt::registry &reg, entt::entity entity)
{
    Networked &networked = reg.get<Networked>(entity);
    std::cout << "Entity " << int(entity) << ":" << networked.networkID << " destroyed\n";
    networkIdToEntity.erase(networked.networkID);

    Packet::from_server::entity_destroyed packet {
            room->getIndexInLevel(),
            networked.networkID
    };
    return mpSession->sendPacketToPlayers(packet, playersInRoom);
}

void NetworkingSystem::update(double deltaTime, Room *room)
{
    room->entities.view<Networked>().each([&](auto e, Networked &networked) {

        for (auto &c : networked.toReceive.list)
        {
            gu::profiler::Zone z(c->getDataTypeName());
            c->update(deltaTime, e, room->entities);
        }

        for (auto &c : networked.toSend.list)
        {
            gu::profiler::Zone z(c->getDataTypeName());
            c->update(deltaTime, e, room->entities);

            json jsonToSend;
            bool hasChanged = false, isPresent = true;
            c->dataToJsonIfChanged(hasChanged, isPresent, jsonToSend, e, room->entities);

            bool wasPresent = networked.dataPresence[c->getDataTypeHash()];
            networked.dataPresence[c->getDataTypeHash()] = isPresent;

            if (wasPresent && !isPresent)
            {
                std::cout << c->getDataTypeName() << " was deleted from " << int(e) << ":" << networked.networkID
                          << "\n";
                Packet::entity_data_removed packet {
                    room->getIndexInLevel(),
                    networked.networkID,
                    c->getDataTypeHash()
                };
                if (mpSession->isServer())
                    mpSession->sendPacketToPlayers(packet, playersInRoom);
                else
                    mpSession->getIOtoServer().send(packet);
            }

            if (hasChanged && isPresent)
            {
                std::cout << c->getDataTypeName() << " changed for " << int(e) << ":" << networked.networkID << ":\n";// << jsonToSend << "\n";

                auto p = dataUpdatePacket(networked, jsonToSend, c);
                if (mpSession->isServer())
                    mpSession->sendPacketToPlayers(p, playersInRoom);
                else
                    mpSession->getIOtoServer().send(p);
            }
        }
    });
}

void NetworkingSystem::handleDataUpdate(Packet::entity_data_update *update, const Player *receivedFrom)
{
    auto it = networkIdToEntity.find(update->entityNetworkId);
    if (it == networkIdToEntity.end())
        throw gu_err("Received entity_data_update for NON-EXISTING entity");

    entt::entity e = it->second;
    Networked &networked = room->entities.get<Networked>(e);

    NetworkedData_ptr &dataType = networked.toReceive.hashToType[update->dataTypeHash];
    if (!dataType)
        throw gu_err("Received entity_data_update for Entity that does not receive data of that type");

    if (receivedFrom)
    {
        PlayerControlled *pC = room->entities.try_get<PlayerControlled>(e);
        if (!pC || pC->playerId != receivedFrom->id)
            throw gu_err("Received entity_data_update for Entity that is NOT controlled by sender");
    }
    dataType->updateDataFromNetwork(update->jsonData, e, room->entities);
}

void NetworkingSystem::handleEntityCreation(Packet::from_server::entity_created *packet)
{
    assert(!mpSession->isServer());
    room->getTemplate(packet->entityTemplateHash)->createNetworked(packet->networkId, false);
}

void NetworkingSystem::handleDataRemoval(Packet::entity_data_removed *packet, const Player *receivedFrom)
{
    auto it = networkIdToEntity.find(packet->entityNetworkId);
    if (it == networkIdToEntity.end())
        throw gu_err("Received entity_data_removed for NON-EXISTING entity");

    entt::entity e = it->second;
    Networked &networked = room->entities.get<Networked>(e);

    NetworkedData_ptr &dataType = networked.toReceive.hashToType[packet->dataTypeHash];
    if (!dataType)
        throw gu_err("Received entity_data_removed for Entity that does not receive data of that type");

    if (receivedFrom)
    {
        PlayerControlled *pC = room->entities.try_get<PlayerControlled>(e);
        if (!pC || pC->playerId != receivedFrom->id)
            throw gu_err("Received entity_data_removed for Entity that is NOT controlled by sender");
    }
    dataType->removeData(e, room->entities);
}

void NetworkingSystem::handleEntityDestroyed(Packet::from_server::entity_destroyed *packet)
{
    assert(!mpSession->isServer());
    room->entities.destroy(networkIdToEntity[packet->networkId]);
    networkIdToEntity.erase(packet->networkId);
}

Packet::from_server::entity_created NetworkingSystem::entityCreatedPacket(Networked &networked)
{
    return Packet::from_server::entity_created {
            room->getIndexInLevel(),
            networked.networkID,
            networked.templateHash
    };
}

Packet::entity_data_update
NetworkingSystem::dataUpdatePacket(Networked &networked, json &data, NetworkedData_ptr &dataType)
{
    return Packet::entity_data_update {
            room->getIndexInLevel(),
            networked.networkID,
            dataType->getDataTypeHash(),
            data
    };
}
