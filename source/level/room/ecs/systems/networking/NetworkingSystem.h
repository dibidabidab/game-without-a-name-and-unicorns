
#ifndef GAME_NETWORKINGSYSTEM_H
#define GAME_NETWORKINGSYSTEM_H


#include "../EntitySystem.h"
#include "../../components/Networked.h"
#include "../../../../../multiplayer/session/Player.h"
#include "../../../../../multiplayer/packets.h"

class MultiplayerSession;

class NetworkingSystem : public EntitySystem
{
    MultiplayerSession *mpSession;
    Room *room;
    std::list<Player_ptr> playersInRoom;
    std::map<int, entt::entity> networkIdToEntity;

  public:
    NetworkingSystem(MultiplayerSession *mpSession) : EntitySystem("network"), mpSession(mpSession)
    {}

    void handleEntityCreation(Packet::from_server::entity_created *packet);

    void handleEntityDestroyed(Packet::from_server::entity_destroyed *packet);

    void handleDataUpdate(Packet::entity_data_update *, const Player *receivedFrom=NULL);

    void handleDataRemoval(Packet::entity_data_removed *, const Player *receivedFrom=NULL);

  protected:
    void init(Room *) override;

    void onNetworkedEntityCreated(entt::registry &, entt::entity);

    void onPlayerEnteredRoom(entt::registry &, entt::entity);

    void onPlayerLeftRoom(entt::registry &, entt::entity);

    void onNetworkedEntityDestroyed(entt::registry &, entt::entity);

    void update(double deltaTime, Room *) override;

    void sendIfNeeded(NetworkedData_ptr &, entt::entity, Networked &);

    Packet::from_server::entity_created entityCreatedPacket(Networked &);

    Packet::entity_data_update dataUpdatePacket(Networked &, json &data, NetworkedData_ptr &dataType);

    NetworkedData_ptr getDataType(bool receiving, entt::entity, Networked&, int dataTypeHash);

};


#endif
