
#ifndef GAME_LUAENTITYTEMPLATE_H
#define GAME_LUAENTITYTEMPLATE_H

#include "EntityTemplate.h"

#include <utility>
#include "../../room/Room.h"
#include "../../../luau.h"

struct LuaEntityScript
{
    std::string source;

    LuaEntityScript(std::string source);

  private:
    sol::bytecode bytecode;
    friend class LuaEntityTemplate;
};

class LuaEntityTemplate : public EntityTemplate
{

    sol::environment env;
    entt::entity currentlyCreating = entt::null;

  public:
    asset<LuaEntityScript> script;

    LuaEntityTemplate(const char *assetName, Room *room);

    static sol::state &getLuaState();

  protected:

    void createComponents(entt::entity entity) override;

    void createComponentsFromScript(entt::entity, sol::optional<sol::table> arguments);

    void luaTableToComponents(entt::entity, const sol::table &);

};


#endif
