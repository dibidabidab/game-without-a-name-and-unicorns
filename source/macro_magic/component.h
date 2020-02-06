
#ifndef GAME_COMPONENT_H
#define GAME_COMPONENT_H

#include "json_reflectable.h"

#define FIELD(type, name, default_value) (type) name

#define COMPONENT(component_name, ...)\
    struct component_name {\
        constexpr static const char *COMPONENT_NAME = #component_name;\
        \
        inline static const size_t COMPONENT_TYPE_HASH = std::hash<std::string_view>{}(#component_name);\
        \
        REFLECTABLE(__VA_ARGS__)\
        \
        size_t getHash()\
        {\
            return 0;\
        }\
        \
        size_t prevHash = 0;\
    };

COMPONENT(
    Test,

    FIELD(int, poep, 3),
    FIELD(float, kaas, 3.0)
)

//COMPONENT(
//    Test2,
//    (int) henkie
//)

#endif //GAME_COMPONENT_H
