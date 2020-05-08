
#ifndef GAME_JSON_CONVERTERS_H
#define GAME_JSON_CONVERTERS_H

#include <utils/math_utils.h>
#include <asset_manager/asset.h>
#include <json.hpp>

namespace nlohmann {
    template <int len, typename type, qualifier something>
    struct adl_serializer<glm::vec<len, type, something>> {
        static void to_json(json& j, const glm::vec<len, type, something>& v) {
            j = json::array();
            for (int i = 0; i < len; i++)
                j[i] = v[i];
        }

        static void from_json(const json& j, glm::vec<len, type, something>& v) {
            for (int i = 0; i < len; i++)
                v[i] = j.at(i);
        }
    };
}

namespace nlohmann {
    template <typename type>
    struct adl_serializer<asset<type>> {
        static void to_json(json& j, const asset<type>& v) {
            if (v.isSet())
                j = v.getLoadedAsset().shortPath;
            else j = "";
        }

        static void from_json(const json& j, asset<type>& v) {
            if (!j.empty())
                v.set(j);
        }
    };
}

#endif //GAME_JSON_CONVERTERS_H
