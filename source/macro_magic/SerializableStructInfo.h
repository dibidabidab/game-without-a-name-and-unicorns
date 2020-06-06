
#ifndef GAME_SERIALIZABLESTRUCTINFO_H
#define GAME_SERIALIZABLESTRUCTINFO_H

#include <vector>
#include <map>
#include "../luau.h"

struct SerializableStructInfo
{
    const char *name;

    typedef std::vector<const char *> str_vec;
    typedef std::vector<bool> bool_vec;

    const str_vec
            fieldNames,      // names of the fields in the struct
            fieldTypeNames;  // types of the fields in the struct

    const bool_vec
            fieldIsPrimitive,          // true for fields that have a primitive value (numbers, booleans, strings, null)
            structFieldIsFixedSize;    // true for fields that are structured (not primitive) but cannot be extended. e.g.: vec3 must always be an array of length 3

    const int nrOfFields;

    typedef std::function<void(void *, const sol::table &table)> fromLuaFunc;

    fromLuaFunc fromLuaTableFunction;

    SerializableStructInfo(
            const char *name, const str_vec &fieldNames, const str_vec &fieldTypeNames,
            const bool_vec &fieldIsStructured,
            const bool_vec &structFieldIsFixedSize,

            fromLuaFunc fromLuaTableFunction
    )

            : name(name), fieldNames(fieldNames), fieldTypeNames(fieldTypeNames),
              fieldIsPrimitive(fieldIsStructured),
              structFieldIsFixedSize(structFieldIsFixedSize),
              nrOfFields(fieldNames.size()),

              fromLuaTableFunction(fromLuaTableFunction)
    {
        if (infos == NULL)
            infos = new std::map<std::string, SerializableStructInfo *>();

        infos->operator[](name) = this;
    }

    static const SerializableStructInfo *getFor(const char *typeName)
    {
        return infos->operator[](typeName);
    }

  private:
    static std::map<std::string, SerializableStructInfo *> *infos;

};

#endif
