
#ifndef GAME_JSON_REFLECTABLE_H
#define GAME_JSON_REFLECTABLE_H

#include "macro_helpers.h"
#include <json.hpp>
#include <utils/gu_error.h>

struct ReflectableStructInfo
{
    const char *name;

    typedef std::vector<const char *> str_vec;
    typedef std::vector<bool> bool_vec;
    const str_vec
        fieldNames,      // names of the fields in the struct
        fieldTypeNames;  // types of the fields in the struct

    const bool_vec
        fieldIsPrimitive,          // true for fields that have a primitive value (numbers, booleans, strings, null)
        structFieldIsFixedSize,    // true for fields that are structured (not primitive) but cannot be extended. e.g.: vec3 must always be an array of length 3
        structFieldAcceptsBool,    // true for fields that are structured and can be extended with a bool
        structFieldAcceptsFloat,   // true for fields that are structured and can be extended with a float
        structFieldAcceptsInt,     // true for fields that are structured and can be extended with a int
        structFieldAcceptsString,  // true for fields that are structured and can be extended with a string
        structFieldAcceptsArray,   // true for fields that are structured and can be extended with an array
        structFieldAcceptsObject;  // true for fields that are structured and can be extended with an object

    const int nrOfFields;

    ReflectableStructInfo(
            const char *name, const str_vec &fieldNames, const str_vec &fieldTypeNames,
            const bool_vec &fieldIsStructured,
            const bool_vec &structFieldIsFixedSize,
            const bool_vec &structFieldAcceptsBool,
            const bool_vec &structFieldAcceptsFloat,
            const bool_vec &structFieldAcceptsInt,
            const bool_vec &structFieldAcceptsString,
            const bool_vec &structFieldAcceptsArray,
            const bool_vec &structFieldAcceptsObject
        )

            : name(name), fieldNames(fieldNames), fieldTypeNames(fieldTypeNames),
              fieldIsPrimitive(fieldIsStructured),
              structFieldIsFixedSize(structFieldIsFixedSize),
              structFieldAcceptsBool(structFieldAcceptsBool),
              structFieldAcceptsFloat(structFieldAcceptsFloat),
              structFieldAcceptsInt(structFieldAcceptsInt),
              structFieldAcceptsString(structFieldAcceptsString),
              structFieldAcceptsArray(structFieldAcceptsArray),
              structFieldAcceptsObject(structFieldAcceptsObject),
              nrOfFields(fieldNames.size())
    {
        infos[name] = this;
    }

    static const ReflectableStructInfo *getFor(const char *typeName)
    {
        return infos[typeName];
    }

  private:
    inline static std::map<const char *, ReflectableStructInfo *> infos;

};


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
    ARGNAME(X) = j.at(pull_out_json_i++).get<ARGTYPE(X)>()\

#define ARGPAIR_FROM_FIELD(field) ARGPAIR(EAT field) ARGTYPE(field)

// struct info:
#define PUT_FIELD_NAME_IN_VEC(field) \
    ARGNAME_AS_STRING(EAT field)

#define PUT_FIELD_TYPE_NAME_IN_VEC(field) \
    TOSTRING(ARGTYPE(EAT field))


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

template <class FieldType, class InsertItem>
bool doesStructFieldAcceptType()
{
    json j = FieldType();

    if (!j.is_structured() || !j.empty()) return false;

    if (j.is_array())
        j.push_back(InsertItem());
    else if (j.is_object())
        j["0"] = InsertItem();

    try
    {
        FieldType f = j;
    }
    catch (std::exception&)
    {
        return false;
    }
    return true;
}

#define IS_FIELD_TYPE_PRIMITIVE(field) \
    isFieldTypePrimitive<ARGTYPE(EAT field)>()

#define IS_FIELD_TYPE_FIXED_SIZE(field) \
    isStructFieldFixedSize<ARGTYPE(EAT field)>()

#define DOES_STRUCT_FIELD_ACCEPT_BOOL(field) \
    doesStructFieldAcceptType<ARGTYPE(EAT field), bool>()

#define DOES_STRUCT_FIELD_ACCEPT_FLOAT(field) \
    doesStructFieldAcceptType<ARGTYPE(EAT field), float>()

#define DOES_STRUCT_FIELD_ACCEPT_INT(field) \
    doesStructFieldAcceptType<ARGTYPE(EAT field), int>()

#define DOES_STRUCT_FIELD_ACCEPT_STRING(field) \
    doesStructFieldAcceptType<ARGTYPE(EAT field), std::string>()

#define DOES_STRUCT_FIELD_ACCEPT_ARRAY(field) \
    doesStructFieldAcceptType<ARGTYPE(EAT field), json::array_t>()

#define DOES_STRUCT_FIELD_ACCEPT_OBJECT(field) \
    doesStructFieldAcceptType<ARGTYPE(EAT field), json::object_t>()

#define REFLECTABLE_STRUCT(className, ...)\
    struct className {\
        /* Dump field types and names */                        \
        DOFOREACH_SEMICOLON(ARGPAIR_FROM_FIELD, __VA_ARGS__)               \
        \
        inline static const ReflectableStructInfo STRUCT_INFO = ReflectableStructInfo(#className, {\
            DOFOREACH(PUT_FIELD_NAME_IN_VEC, __VA_ARGS__)\
        }, {\
            DOFOREACH(PUT_FIELD_TYPE_NAME_IN_VEC, __VA_ARGS__)\
        },\
        {\
            DOFOREACH(IS_FIELD_TYPE_PRIMITIVE, __VA_ARGS__)\
        }, {\
            DOFOREACH(IS_FIELD_TYPE_FIXED_SIZE, __VA_ARGS__)\
        }, {\
            DOFOREACH(DOES_STRUCT_FIELD_ACCEPT_BOOL, __VA_ARGS__)\
        }, {\
            DOFOREACH(DOES_STRUCT_FIELD_ACCEPT_FLOAT, __VA_ARGS__)\
        }, {\
            DOFOREACH(DOES_STRUCT_FIELD_ACCEPT_INT, __VA_ARGS__)\
        }, {\
            DOFOREACH(DOES_STRUCT_FIELD_ACCEPT_STRING, __VA_ARGS__)\
        }, {\
            DOFOREACH(DOES_STRUCT_FIELD_ACCEPT_ARRAY, __VA_ARGS__)\
        }, {\
            DOFOREACH(DOES_STRUCT_FIELD_ACCEPT_OBJECT, __VA_ARGS__)\
        });\
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
