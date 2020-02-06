
#ifndef GAME_JSON_REFLECTABLE_H
#define GAME_JSON_REFLECTABLE_H

#include "macro_helpers.h"
#include "json.hpp"

#define FIELD(type, name)                        ()                (type) name

#define FIELD_DEF_VAL(type, name, default_value) (= default_value) (type) name

#define PUT_FIELD_IN_JSON(field) \
    PUT_IN_JSON(EAT field)

#define PULL_FIELD_OUT_JSON(field) \
    PULL_OUT_JSON(EAT field)

#define PUT_IN_JSON(X)  \
    {ARGNAME_AS_STRING(X), ARGNAME(X)}

#define PULL_OUT_JSON(X)  \
    j.at(ARGNAME_AS_STRING(X)).get_to(ARGNAME(X))

#define ARGPAIR_FROM_FIELD(field) ARGPAIR(EAT field) ARGTYPE(field)

#define REFLECTABLE(...)                \
        /* Dump field types and names */                        \
        DOFOREACH_SEMICOLON(ARGPAIR_FROM_FIELD, __VA_ARGS__)               \
        \
        void toJson(json &j) const\
        {\
            j = json{DOFOREACH(PUT_FIELD_IN_JSON, __VA_ARGS__)};\
        }\
        \
        void updateFromJson(const json &j)\
        {\
            DOFOREACH_SEMICOLON(PULL_FIELD_OUT_JSON, __VA_ARGS__)\
        }\

#endif //GAME_JSON_REFLECTABLE_H
