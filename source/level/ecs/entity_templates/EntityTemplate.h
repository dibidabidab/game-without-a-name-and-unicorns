
#ifndef GAME_ENTITYTEMPLATE_H
#define GAME_ENTITYTEMPLATE_H


#include "../../../../entt/src/entt/entity/registry.hpp"

class Room;
class Networked;

class EntityTemplate
{
  protected:
    Room *room = NULL;
    int templateHash = -1;
    friend Room;

  public:
    virtual entt::entity create() = 0;

    entt::entity createNetworked(int networkID=rand(), bool serverSide=true);

    virtual void makeNetworkedServerSide(Networked &) {}

    virtual void makeNetworkedClientSide(Networked &) {}

    virtual ~EntityTemplate() = default;
};


#endif
