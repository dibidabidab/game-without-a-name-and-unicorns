
#ifndef GAME_LUAENTITYTEMPLATE_H
#define GAME_LUAENTITYTEMPLATE_H

#include "EntityTemplate.h"
#include "../../room/Room.h"
#include "../../../luau.h"

struct LuaEntityScript
{
    std::string source;
};

class LuaEntityTemplate : public EntityTemplate
{

    asset<LuaEntityScript> script;

  public:
    LuaEntityTemplate(const char *assetName);

  protected:

    void createComponents(entt::entity entity) override;

    void createComponentsFromScript(entt::entity, sol::state &lua, sol::optional<sol::table> arguments);

    void luaTableToComponents(entt::entity, const sol::table &);

};


#endif
