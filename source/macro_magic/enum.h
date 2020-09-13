
#include <json.hpp>
#include "macro_helpers.h"

#define ENUM_TO_JSON_MAP(val) \
    {std::remove_reference_t<decltype(e)>::val, #val}   // e is an (const reference of ENUM_TYPE) argument in the to_json and from_json functions

#define ENUM(ENUM_TYPE, ...) \
    enum class ENUM_TYPE {\
        __VA_ARGS__\
    };                       \
    NLOHMANN_JSON_SERIALIZE_ENUM(ENUM_TYPE, {DOFOREACH(ENUM_TO_JSON_MAP, __VA_ARGS__)})
