
#ifndef GAME_SERIALIZABLE_H
#define GAME_SERIALIZABLE_H

#include "SerializableStructInfo.h"
#include "lua_converters.h"
#include "json_converters.h"
#include <json.hpp>
#include <utils/gu_error.h>

template <typename T>
inline std::string to_string(
        const T &
)
{
    static auto name = "<" + getTypeName<T>() + ">";
    return name;
}


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

#endif //GAME_SERIALIZABLE_H
