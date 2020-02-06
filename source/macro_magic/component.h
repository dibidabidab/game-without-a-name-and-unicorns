
#ifndef GAME_COMPONENT_H
#define GAME_COMPONENT_H

#include "json_reflectable.h"

template<typename T>
size_t hashValue(const T &v)
{
    return std::hash<T>{}(v);
}

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
    struct component_name {\
        constexpr static const char *COMPONENT_NAME = #component_name;\
        \
        inline static const size_t COMPONENT_TYPE_HASH = std::hash<std::string_view>{}(#component_name);\
        \
        REFLECTABLE(__VA_ARGS__)\
        \
        hash_func\
        \
        size_t prevHash = 0;\
    };


COMPONENT(
    // component name:
    Test,

    // values to calculate hash:
    HASH(ham, kaas),

    // fields:
    FIELD_DEF_VAL (int, ham, 2),
    FIELD         (float, kaas)
)


COMPONENT(
// component name:
        Test2,

// values to calculate hash:
        HASH(ham, kaas),

// fields:
        FIELD_DEF_VAL (int, ham, 2),
        FIELD         (float, kaas)
)

#endif //GAME_COMPONENT_H
