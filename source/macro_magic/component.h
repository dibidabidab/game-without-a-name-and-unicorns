
#ifndef GAME_COMPONENT_H
#define GAME_COMPONENT_H

#include "json_reflectable.h"
#include <utils/hashing.h>

#define HASH_FIELD(X) \
    hash ^= hashValue(X) + 0x9e3779b9 + (hash << 6u) + (hash >> 2u)

#define HASH(...)\
    size_t getHash()\
    {\
        size_t hash = 0;\
        DOFOREACH_SEMICOLON(HASH_FIELD, __VA_ARGS__)\
        return hash;\
    }\


#define COMPONENT(component_name, hash_func, ...)\
    REFLECTABLE_STRUCT(component_name, __VA_ARGS__)\
        \
        constexpr static const char *COMPONENT_NAME = #component_name;\
        \
        inline static const size_t COMPONENT_TYPE_HASH = hashStringCrossPlatform(#component_name);\
        \
        hash_func\
        \
        size_t prevHash = 0;
#define END_COMPONENT(component_name) END_REFLECTABLE_STRUCT(component_name)

COMPONENT(
    // component name:
    Test,

    // values to calculate hash:
    HASH(ham, kaas),

    // fields:
    FIELD_DEF_VAL (int, ham, 2),
    FIELD         (float, kaas)
)
END_COMPONENT(Test)

#endif //GAME_COMPONENT_H
