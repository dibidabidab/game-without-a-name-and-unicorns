
#ifndef GAME_SERIALIZABLE_H
#define GAME_SERIALIZABLE_H

#include "SerializableStructInfo.h"
#include "lua_converters.h"
#include "json_converters.h"
#include <json.hpp>
#include <utils/gu_error.h>

// this template is needed for sol. (probably because sol tries to use glm::to_string())
template <typename T>
inline std::string to_string(
        const T &
)
{
    static auto name = "<" + getTypeName<T>() + ">";
    return name;
}


template <class FieldType>
bool isFieldTypePrimitive()
{
    json j = FieldType();
    return !j.is_structured();
}

/**
 * assume that a FieldType is fixed size if a new instance already contains items.
 *
 * if there are no items, then assume that FieldType will accept new items.     // kinda TODO. this is EXTREMELY hacky, but so far it works perfect
 */
template <class FieldType>
bool isStructFieldFixedSize()
{
    json j = FieldType();
    return j.is_structured() && !j.empty();
}

#endif //GAME_SERIALIZABLE_H
