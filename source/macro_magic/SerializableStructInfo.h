
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

    std::function<void(sol::state &)> luaUserTypeGenerator;

    SerializableStructInfo(
            const char *name, const str_vec &fieldNames, const str_vec &fieldTypeNames,
            const bool_vec &fieldIsStructured,
            const bool_vec &structFieldIsFixedSize,

            std::size_t typeHash,

            std::function<void(sol::state &)> luaUserTypeGenerator
    )

            : name(name), fieldNames(fieldNames), fieldTypeNames(fieldTypeNames),
              fieldIsPrimitive(fieldIsStructured),
              structFieldIsFixedSize(structFieldIsFixedSize),
              nrOfFields(fieldNames.size()),
              luaUserTypeGenerator(luaUserTypeGenerator)
    {
        if (infos == NULL)
            infos = new std::map<std::string, SerializableStructInfo *>();
        if (infosByType == NULL)
            infosByType = new std::map<std::size_t, SerializableStructInfo *>();

        infos->operator[](name) = this;
        infosByType->operator[](typeHash) = this;
    }

    static const SerializableStructInfo *getFor(const char *typeName)
    {
        return infos->operator[](typeName);
    }

    template<typename type>
    static const SerializableStructInfo *getFor()
    {
        return infosByType->operator[](typeid(type).hash_code());
    }

    template<typename StructType>
    static std::size_t getTypeHash()
    {
        return typeid(StructType).hash_code();
    }

    static const std::map<std::string, SerializableStructInfo *> &getForAllTypes()
    {
        return *infos;
    }

  private:
    static std::map<std::string, SerializableStructInfo *> *infos;
    static std::map<size_t, SerializableStructInfo *> *infosByType;

};

#endif
