
#include <ecs/EntityEngine.h>
#include "CombatSystem.h"
#include "../../../generated/PlayerDetector.hpp"
#include "../../../generated/Physics.hpp"
#include <generated/PlayerControlled.hpp>

void CombatSystem::update(double deltaTime, EntityEngine *engine)
{

    engine->entities.view<PlayerDetector, AABB>().each([&] (auto detectorEntity, PlayerDetector &detector, AABB &detectorAABB) {

        float distSq = detector.distance * detector.distance;
        auto prevPlayers = detector.detectedPlayerEntities;
        detector.detectedPlayerEntities.clear();

        bool anyNew = false;

        engine->entities.view<PlayerControlled, AABB>().each([&] (auto player, auto &, AABB &playerAABB) {

            vec2 diff = playerAABB.center - detectorAABB.center;
            if (length2(diff) <= distSq)
            {
                detector.detectedPlayerEntities.push_back(player);

                bool justEnteredRange = true;
                for (auto pl : prevPlayers)
                    if (pl == player)
                    {
                        justEnteredRange = false;
                        break;
                    }

                if (justEnteredRange)
                {
                    engine->emitEntityEvent(detectorEntity, player, "PlayerDetected");
                    anyNew = true;
                }
            }
        });

        if (prevPlayers.size() == detector.detectedPlayerEntities.size() && !anyNew)
            return; // same players.

        // something changed, check for players that left:
        for (auto playerPrev : prevPlayers)
        {
            bool gone = true;
            for (auto pl : detector.detectedPlayerEntities)
                if (pl == playerPrev)
                {
                    gone = false;
                    break;
                }

            if (gone)
                engine->emitEntityEvent(detectorEntity, playerPrev, "PlayerGone");
        }
    });

}
