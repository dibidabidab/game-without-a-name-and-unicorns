
#ifndef GAME_SERIALIZABLE_H
#define GAME_SERIALIZABLE_H

#include "SerializableStructInfo.h"
#include "lua_converters.h"
#include "macro_helpers.h"
#include "json_converters.h"
#include <json.hpp>
#include <utils/gu_error.h>


#define FIELD(type, name)                        ()                (type)name

#define FIELD_DEF_VAL(type, name, default_value) (= default_value) (type)name

#define PUT_FIELD_IN_JSON(field) \
    PUT_IN_JSON(EAT field)

#define PULL_FIELD_OUT_JSON(field) \
    PULL_OUT_JSON(EAT field)

#define PULL_OUT_JSON(X) \
    if (j.contains(ARGNAME_AS_STRING(X)))   \
        getTo(ARGNAME(X), j.at(ARGNAME_AS_STRING(X)))

#define COPY_FROM_OTHER(field) \
    ARGNAME(EAT field) = other.ARGNAME(EAT field)

#define PUT_IN_JSON(X)  \
    {ARGNAME_AS_STRING(X), ARGNAME(X)}

// array:
#define PUT_FIELD_IN_JSON_ARRAY(field) \
    ARGNAME(EAT field)

#define PULL_FIELD_OUT_JSON_ARRAY(field) \
    PULL_OUT_JSON_ARRAY(EAT field)

template <typename T>
inline void getTo(T &v, const json &json)
{
    v = json.get<T>();
}
template <>
inline void getTo<json>(json &v, const json &json)
{
    v = json;
}

#define PULL_OUT_JSON_ARRAY(X)  \
    getTo(ARGNAME(X), j.at(pull_out_json_i++))

#define ARGPAIR_FROM_FIELD(field) ARGPAIR(EAT field) ARGTYPE(field)

// struct info:
#define PUT_FIELD_NAME_IN_VEC(field) \
    ARGNAME_AS_STRING(EAT field)

#define TOSTRING_(f) \
    TOSTRING(f)

#define TOSTRING__(f) \
    TOSTRING_(f)

#define PUT_FIELD_TYPE_NAME_IN_VEC(field) \
    TOSTRING__(ARGTYPE(EAT field))


template <class FieldType>
bool isFieldTypePrimitive()
{
    json j = FieldType();
    return !j.is_structured();
}

/**
 * assume that a FieldType is fixed size if a new instance already contains items.
 *
 * if there are no items, then assume that FieldType will accept new items.
 */
template <class FieldType>
bool isStructFieldFixedSize()
{
    json j = FieldType();
    return j.is_structured() && !j.empty();
}

#define IS_FIELD_TYPE_PRIMITIVE(field) \
    isFieldTypePrimitive<ARGTYPE(EAT field)>()

#define IS_FIELD_TYPE_FIXED_SIZE(field) \
    isStructFieldFixedSize<ARGTYPE(EAT field)>()

#define SERIALIZABLE(className, ...)\
    struct className {\
        /* Dump field types and names */                        \
        DOFOREACH_SEMICOLON(ARGPAIR_FROM_FIELD, __VA_ARGS__)               \
        \
        void fromLuaTable(const sol::table &table)\
        {\
            FROM_LUA_TABLE(__VA_ARGS__)\
        }\
        void toLuaTable(sol::table &table) const\
        {\
            TO_LUA_TABLE(__VA_ARGS__)\
        }\
        \
        inline static const SerializableStructInfo STRUCT_INFO = SerializableStructInfo(#className, {\
            DOFOREACH(PUT_FIELD_NAME_IN_VEC, __VA_ARGS__)\
        }, {\
            DOFOREACH(PUT_FIELD_TYPE_NAME_IN_VEC, __VA_ARGS__)\
        },\
        {\
            DOFOREACH(IS_FIELD_TYPE_PRIMITIVE, __VA_ARGS__)\
        }, {\
            DOFOREACH(IS_FIELD_TYPE_FIXED_SIZE, __VA_ARGS__)\
        },\
            SerializableStructInfo::getTypeHash<className>()\
        );\
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
        void copyFieldsFrom(const className &other)\
        {\
            DOFOREACH_SEMICOLON(COPY_FROM_OTHER, __VA_ARGS__)\
        }

#define END_SERIALIZABLE(className)\
    };\
    static void to_json(json& j, const className& v) {\
        v.toJsonArray(j);\
    }\
    \
    static void from_json(const json& j, className& v) {\
        v.fromJsonArray(j);\
    }

#define END_SERIALIZABLE_FULL_JSON(className)\
    };\
    static void to_json(json& j, const className& v) {\
        v.toJson(j);\
    }\
    \
    static void from_json(const json& j, className& v) {\
        v.fromJson(j);\
    }

#endif //GAME_SERIALIZABLE_H
