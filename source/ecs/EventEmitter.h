
#ifndef GAME_EVENTEMITTER_H
#define GAME_EVENTEMITTER_H

#include <unordered_map>
#include <list>
#include <utils/type_name.h>
#include "../../external/entt/src/entt/core/hashed_string.hpp"
#include "../luau.h"
#include "../macro_magic/lua_converters.h"

class EventEmitter
{

    using hash_type = entt::hashed_string::hash_type;

    std::unordered_map<hash_type, std::list<sol::function>> eventListeners;

  public:

    template<typename type>
    void emit(const type &event)
    {
        static hash_type typeHash = 0;
        if (typeHash == 0)
            typeHash = entt::hashed_string { getTypeName<type>().c_str() }.value();

        auto &listeners = eventListeners[typeHash];

        if (listeners.empty())
            return;

        // convert event object to a Lua object:
        static auto luaRef = luau::getLuaState()["__current_event__"];
        lua_converter<type>::toLua(luaRef, event);

        auto it = listeners.begin();

        bool removeListener = false;
        auto removeCallback = [&] {
            removeListener = true;
        };

        // call each listener with the event as argument:
        // also pass a callback function that can be used to remove the listener
        while (it != listeners.end())
        {
            auto &listener = *it;
            listener(luaRef, removeCallback);

            if (removeListener)
            {
                removeListener = false;
                it = listeners.erase(it);
            }
            else ++it;
        }
    }

    void on(const char *eventName, const sol::function &listener)
    {
        auto typeHash = entt::hashed_string { eventName }.value();

        auto &listeners = eventListeners[typeHash];

        listeners.push_back(listener);
    }

};


#endif
