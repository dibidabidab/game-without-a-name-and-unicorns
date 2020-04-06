
#ifndef GAME_JSON_REFLECTABLE_H
#define GAME_JSON_REFLECTABLE_H

#include "macro_helpers.h"
#include "json.hpp"
#include "utils/gu_error.h"

#define FIELDS(type, ...) DOFOREACH( , __VA_ARGS__)

#define FIELD(type, name)                        ()                (type) name

#define FIELD_DEF_VAL(type, name, default_value) (= default_value) (type) name

#define PUT_FIELD_IN_JSON(field) \
    PUT_IN_JSON(EAT field)

#define PULL_FIELD_OUT_JSON(field) \
    PULL_OUT_JSON(EAT field)

#define IF_KEY_PULL_FIELD_OUT_JSON(field) \
    IF_KEY_PULL_OUT_JSON(EAT field)

#define PUT_IN_JSON(X)  \
    {ARGNAME_AS_STRING(X), ARGNAME(X)}

#define PULL_OUT_JSON(X)  \
    j.at(ARGNAME_AS_STRING(X)).get_to(ARGNAME(X))

#define IF_KEY_PULL_OUT_JSON(X)  \
    if (key == ARGNAME_AS_STRING(X))\
        ARGNAME(X) = el.value();\
    else


// array:
#define PUT_FIELD_IN_JSON_ARRAY(field) \
    ARGNAME(EAT field)

#define PULL_FIELD_OUT_JSON_ARRAY(field) \
    PULL_OUT_JSON_ARRAY(EAT field)

#define PULL_OUT_JSON_ARRAY(X)  \
    j.at(pull_out_json_i++).get_to(ARGNAME(X))

#define ARGPAIR_FROM_FIELD(field) ARGPAIR(EAT field) ARGTYPE(field)

#define REFLECTABLE_STRUCT(className, ...)\
    struct className {\
        /* Dump field types and names */                        \
        DOFOREACH_SEMICOLON(ARGPAIR_FROM_FIELD, __VA_ARGS__)               \
        \
        void toJson(json &j) const\
        {\
            j = json{DOFOREACH(PUT_FIELD_IN_JSON, __VA_ARGS__)};\
        }\
        void toJsonArray(json &j) const\
        {\
            j = json::array({DOFOREACH(PUT_FIELD_IN_JSON_ARRAY, __VA_ARGS__)});\
        }\
        \
        void fromJson(const json &j)\
        {\
            DOFOREACH_SEMICOLON(PULL_FIELD_OUT_JSON, __VA_ARGS__)\
        }\
        \
        void fromJsonArray(const json &j)\
        {\
            int pull_out_json_i = 0;\
            DOFOREACH_SEMICOLON(PULL_FIELD_OUT_JSON_ARRAY, __VA_ARGS__)\
        }\
        \
//        void updatePartiallyFromJson(const json &j)\
//        {\
//            for (auto &el : j.items())\
//            {\
//                auto &key = el.key();\
//                DOFOREACH_NO_DELIMITER(IF_KEY_PULL_FIELD_OUT_JSON, __VA_ARGS__)\
//                {\
//                    throw gu_err(key + " is not a member of " + #className);\
//                }\
//            }\
//        }

#define END_REFLECTABLE_STRUCT(className)\
    };\
    static void to_json(json& j, const className& v) {\
        v.toJsonArray(j);\
    }\
    \
    static void from_json(const json& j, className& v) {\
        v.fromJsonArray(j);\
    }

#endif //GAME_JSON_REFLECTABLE_H
