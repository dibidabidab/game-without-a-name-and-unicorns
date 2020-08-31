
#include "LuaScriptsSystem.h"

void LuaScriptsSystem::init(EntityEngine *room)
{
    room->entities.on_destroy<LuaScripted>().connect<&LuaScriptsSystem::onDestroyed>(this);
}

void LuaScriptsSystem::update(double deltaTime, EntityEngine *room)
{
    room->entities.view<LuaScripted>().each([&](auto e, LuaScripted &scripted) {

        if (scripted.updateFrequency <= 0)
            callUpdateFunc(e, scripted, deltaTime);
        else
        {
            scripted.updateAccumulator += deltaTime;
            while (scripted.updateAccumulator > scripted.updateFrequency)
            {
                scripted.updateAccumulator -= scripted.updateFrequency;
                callUpdateFunc(e, scripted, scripted.updateFrequency);
                if (!room->entities.valid(e) || scripted.updateFrequency <= 0)
                    return;
            }
        }
    });
}

void LuaScriptsSystem::callUpdateFunc(entt::entity e, LuaScripted &scripted, float deltaTime)
{
    if (!scripted.updateFunc.valid() || scripted.updateFunc.is<sol::nil_t>())
        return;

    try
    {
        sol::protected_function_result result = scripted.updateFunc(deltaTime, e);
        if (!result.valid())
            throw gu_err(result.get<sol::error>().what());
    }
    catch (std::exception &exc)
    {
        assert(scripted.updateFuncScript.isSet());
        std::cerr << "Error while calling Lua update function for entity#" << int(e) << " (" << scripted.updateFuncScript.getLoadedAsset().fullPath << "):" << std::endl;
        std::cerr << exc.what() << std::endl;
    }
}

void LuaScriptsSystem::onDestroyed(entt::registry &reg, entt::entity e)
{
    LuaScripted &scripted = reg.get<LuaScripted>(e);
    if (!scripted.onDestroyFunc.valid() || scripted.onDestroyFunc.is<sol::nil_t>())
        return;

    try
    {
        sol::protected_function_result result = scripted.onDestroyFunc(e);
        if (!result.valid())
            throw gu_err(result.get<sol::error>().what());
    }
    catch (std::exception &exc)
    {
        assert(scripted.onDestroyFuncScript.isSet());
        std::cerr << "Error while calling Lua onDestroy callback for entity#" << int(e) << " (" << scripted.onDestroyFuncScript.getLoadedAsset().fullPath << "):" << std::endl;
        std::cerr << exc.what() << std::endl;
    }
}
