
#ifndef GAME_JSON_CONVERTERS_H
#define GAME_JSON_CONVERTERS_H

#include <utils/math_utils.h>
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

#endif //GAME_JSON_CONVERTERS_H
