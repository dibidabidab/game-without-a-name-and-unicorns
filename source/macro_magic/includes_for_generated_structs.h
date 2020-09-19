
#ifndef IFGS
#define IFGS

#include "component.h"
#include <audio/audio.h>
#include <input/key_input.h>
#include "../ecs/entity_templates/LuaEntityTemplate.h"

#include "../ecs/systems/networking/NetworkedDataList.h"
#include "../ecs/systems/networking/NetworkedDataGroup.h"

#include <set>
#include <imgui.h>

struct AABB;
#include "../ecs/systems/physics/TerrainCollisionDetector.h"

template <typename T>
inline std::string to_string(
        const T &
)
{
    return "lol";
}

#endif
