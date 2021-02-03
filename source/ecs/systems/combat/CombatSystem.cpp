
#include <ecs/EntityEngine.h>
#include "CombatSystem.h"
#include "../../../generated/PlayerDetector.hpp"
#include "../../../generated/Physics.hpp"
#include <generated/PlayerControlled.hpp>

void CombatSystem::update(double deltaTime, EntityEngine *engine)
{

    engine->entities.view<PlayerDetector, AABB>().each([&] (auto detectorEntity, PlayerDetector &detector, AABB &detectorAABB) {

        float distSq = detector.distance * detector.distance;

        engine->entities.view<PlayerControlled, AABB>().each([&] (auto player, auto &, AABB &playerAABB) {

            vec2 diff = playerAABB.center - detectorAABB.center;
            if (length2(diff) <= distSq)
                engine->emitEntityEvent(detectorEntity, player, "PlayerDetected");
        });

    });

}
