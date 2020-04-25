
#ifndef GAME_ENTITYTEMPLATE_H
#define GAME_ENTITYTEMPLATE_H


#include "../../../../entt/src/entt/entity/registry.hpp"

class Room;
class Networked;

class EntityTemplate
{
  private:
    friend class Room;
    int templateHash = -1;

  protected:
    Room *room = NULL;

  public:
    virtual entt::entity create() = 0;

    entt::entity createNetworked(int networkID=rand(), bool serverSide=true);

    virtual void makeNetworkedServerSide(Networked &) {}

    virtual void makeNetworkedClientSide(Networked &) {}

    virtual ~EntityTemplate() = default;
};


#endif
