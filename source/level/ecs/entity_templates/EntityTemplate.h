
#ifndef GAME_ENTITYTEMPLATE_H
#define GAME_ENTITYTEMPLATE_H


#include "../../../../external/entt/src/entt/entity/registry.hpp"

class Room;
class Networked;

/**
 * Abstract class.
 *
 * Entity templates are used to construct one (or more) entities with a collection of components.
 *
 * .create() is used to create the entity as if it only exists client-side.
 *
 * .createNetworked() is used to create the entity as if it exists on all clients.
 */
class EntityTemplate
{
  private:
    friend class Room;
    int templateHash = -1;

  protected:
    Room *room = NULL;

  public:

    entt::entity create();

    entt::entity createNetworked(int networkID=rand(), bool serverSide=true);

    virtual void createComponents(entt::entity) = 0;

  protected:

    virtual void makeNetworkedServerSide(Networked &) {}

    virtual void makeNetworkedClientSide(Networked &) {}

    virtual ~EntityTemplate() = default;

    entt::entity createChild(entt::entity parent, const char *childName="");

    void setParent(entt::entity child, entt::entity parent, const char *childName="");

};


#endif
