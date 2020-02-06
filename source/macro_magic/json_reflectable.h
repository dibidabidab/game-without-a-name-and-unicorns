
#ifndef GAME_JSON_REFLECTABLE_H
#define GAME_JSON_REFLECTABLE_H

#include "macro_helpers.h"
#include "json.hpp"

#define PUT_IN_JSON(X)  \
    {ARGNAME_AS_STRING(X), ARGNAME(X)}

#define PULL_OUT_JSON(X)  \
    j.at(ARGNAME_AS_STRING(X)).get_to(ARGNAME(X))

#define REFLECTABLE(...)                \
        /* Dump field types and names */                        \
        DOFOREACH_SEMICOLON(ARGPAIR, __VA_ARGS__)               \
        \
        void toJson(json &j) const\
        {\
            j = json{DOFOREACH(PUT_IN_JSON, __VA_ARGS__)};\
        }\
        \
        void updateFromJson(const json &j)\
        {\
            DOFOREACH_SEMICOLON(PULL_OUT_JSON, __VA_ARGS__)\
        }\

struct Person
{
    REFLECTABLE(
        (std::string) name,
        (int) age,
        (bool) isAdult
    )
};

#endif //GAME_JSON_REFLECTABLE_H
