
#include "LuaScriptsSystem.h"

void LuaScriptsSystem::update(double deltaTime, Room *room)
{
    room->entities.view<LuaScripted>().each([&](auto e, LuaScripted &scripted) {

        if (!scripted.updateFunc.valid())
            return;

        if (scripted.updateFrequency <= 0)
            callUpdateFunc(e, scripted, deltaTime);
        else
        {
            scripted.updateAccumulator += deltaTime;
            while (scripted.updateAccumulator > scripted.updateFrequency)
            {
                scripted.updateAccumulator -= scripted.updateFrequency;
                callUpdateFunc(e, scripted, scripted.updateFrequency);
            }
        }
    });
}

void LuaScriptsSystem::callUpdateFunc(entt::entity e, LuaScripted &scripted, float deltaTime)
{
    try
    {
        sol::protected_function_result result = scripted.updateFunc(e, deltaTime);
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
