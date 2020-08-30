
#ifndef GAME_LUAENTITYTEMPLATE_H
#define GAME_LUAENTITYTEMPLATE_H

#include "EntityTemplate.h"

#include <utility>
#include "../../Room.h"
#include "../../../../luau.h"
#include "../../../../macro_magic/component.h"
#include "../components/Saving.h"

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

    sol::function createFunc, onDestroyFunc;
    std::string description;
    sol::table defaultArgs;

    Persistent persistency;
    bool persistentArgs = false;

  public:
    asset<LuaEntityScript> script;

    LuaEntityTemplate(const char *assetName, const char *name, Room *room);

    static sol::state &getLuaState();

    const std::string &getDescription();

    json getDefaultArgs();

    void createComponents(entt::entity, bool persistent) override;

    void createComponentsWithJsonArguments(entt::entity, const json &arguments, bool persistent);

    void createComponentsWithLuaArguments(entt::entity, sol::optional<sol::table> arguments, bool persistent);

  protected:

    void runScript();

    void luaTableToComponent(entt::entity, const std::string &componentName, const sol::table &);

    const ComponentUtils &componentUtils(const std::string &componentName);

};


#endif
