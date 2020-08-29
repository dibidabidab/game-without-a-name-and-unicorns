
#ifndef GAME_LUASCRIPTSSYSTEM_H
#define GAME_LUASCRIPTSSYSTEM_H

#include "EntitySystem.h"
#include "../../Room.h"
#include "../components/LuaScript.h"

class LuaScriptsSystem : public EntitySystem
{
    using EntitySystem::EntitySystem;
  protected:
    void update(double deltaTime, Room *room) override;

    void callUpdateFunc(entt::entity, LuaScripted &, float deltaTime);

};


#endif
