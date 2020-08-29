
#ifndef GAME_LUASCRIPTSSYSTEM_H
#define GAME_LUASCRIPTSSYSTEM_H

#include "EntitySystem.h"
#include "../../Room.h"
#include "../components/LuaScript.h"

class LuaScriptsSystem : public EntitySystem
{
    using EntitySystem::EntitySystem;
  protected:
    void init(Room *room) override;

    void update(double deltaTime, Room *room) override;

    void callUpdateFunc(entt::entity, LuaScripted &, float deltaTime);

    void onDestroyed(entt::registry &, entt::entity);

};


#endif
