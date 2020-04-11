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
    networkIdToEntity[networked.networkID] = entity;

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

bool NetworkingSystem::sendCreatedEntity(Networked &networked, const Player_ptr &player)
{
    Packet::from_server::entity_created packet {
            room->getIndexInLevel(),
            networked.networkID,
            networked.templateHash
    };
    return mpSession->sendPacketToPlayer(packet, player);
}

void NetworkingSystem::onPlayerEnteredRoom(entt::registry &reg, entt::entity entity)
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

void NetworkingSystem::onPlayerLeftRoom(entt::registry &reg, entt::entity entity)
{
    PlayerControlled &pC = reg.get<PlayerControlled>(entity);
    int sizeBefore = playersInRoom.size();
    playersInRoom.remove_if([&](auto &p) { return pC.playerId == p->id; });
    assert(sizeBefore == playersInRoom.size() + 1);
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
    return mpSession->sendPacketToAllPlayers(packet);
}

void NetworkingSystem::update(double deltaTime, Room *room)
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
            networked.dataPresence[c->getDataTypeHash()] = isPresent;

            if (wasPresent && !isPresent)
            {
                std::cout << c->getDataTypeName() << " was deleted from " << int(e) << ":" << networked.networkID <<"\n";
                Packet::entity_data_removed packet;
                packet.dataTypeHash = c->getDataTypeHash();
                packet.entityNetworkId = networked.networkID;
                packet.roomI = room->getIndexInLevel();
                mpSession->sendPacketToAllPlayers(packet);
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

bool NetworkingSystem::sendEntityDataUpdate(Networked &networked, json &data, AbstractNetworkedData *dataType,
                                            const Player_ptr &sendTo)
{
    Packet::entity_data_update packet;
    packet.roomI = room->getIndexInLevel();
    packet.dataTypeHash = dataType->getDataTypeHash();
    packet.entityNetworkId = networked.networkID;
    packet.jsonData = data;
    return mpSession->sendPacketToPlayer(packet, sendTo);
}

void NetworkingSystem::handleDataUpdate(Packet::entity_data_update *update, const Player *receivedFrom)
{
    auto it = networkIdToEntity.find(update->entityNetworkId);
    if (it == networkIdToEntity.end())
        throw gu_err("Received entity_data_update for NON-EXISTING entity that has networkID "
                    + std::to_string(update->entityNetworkId));

    entt::entity e = it->second;
    Networked &networked = room->entities.get<Networked>(e);
    if (!networked.toReceive)
        throw gu_err("Received entity_data_update for Entity that does not receive data");

    AbstractNetworkedData *dataType = networked.toReceive->hashToType[update->dataTypeHash];
    if (!dataType)
        throw gu_err(// todo replace with normal exception that doesnt print automatically?
                "Received entity_data_update for Entity that does not receive data of type "
                + std::to_string(update->dataTypeHash));

    if (receivedFrom)
    {
        PlayerControlled *pC = room->entities.try_get<PlayerControlled>(e);
        if (!pC || pC->playerId != receivedFrom->id)
            throw gu_err("Received entity_data_update for Entity that is NOT controlled by " + receivedFrom->name);
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
        throw gu_err("Received entity_data_removed for NON-EXISTING entity that has networkID "
                     + std::to_string(packet->entityNetworkId));

    entt::entity e = it->second;
    Networked &networked = room->entities.get<Networked>(e);
    if (!networked.toReceive)
        throw gu_err("Received entity_data_removed for Entity that does not receive data");

    AbstractNetworkedData *dataType = networked.toReceive->hashToType[packet->dataTypeHash];
    if (!dataType)
        throw gu_err(// todo replace with normal exception that doesnt print automatically?
                "Received entity_data_removed for Entity that does not receive data of type "
                + std::to_string(packet->dataTypeHash));

    if (receivedFrom)
    {
        PlayerControlled *pC = room->entities.try_get<PlayerControlled>(e);
        if (!pC || pC->playerId != receivedFrom->id)
            throw gu_err("Received entity_data_removed for Entity that is NOT controlled by " + receivedFrom->name);
    }
    dataType->removeData(e, room->entities);
}

void NetworkingSystem::handleEntityDestroyed(Packet::from_server::entity_destroyed *packet)
{
    assert(!mpSession->isServer());
    room->entities.destroy(networkIdToEntity[packet->networkId]);
    networkIdToEntity.erase(packet->networkId);
}
