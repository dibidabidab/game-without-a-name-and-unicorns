
#include "LightSystem.h"
#include "../../../room/Room.h"
#include "../../components/graphics/Light.h"

void LightSystem::update(double deltaTime, Room *room)
{
    room->entities.view<LightPoint>().each([&](LightPoint &l) {

        if (l.radiusFlickeringFrequency == 0)
        {
            l.flickeringRadius = 0;
            return;
        }

        l.flickeringTimeRemaining -= deltaTime;
        if (
            l.flickeringTimeRemaining <= 0
            ||
            1. / l.radiusFlickeringFrequency < l.flickeringTimeRemaining // lamp might be stuck in a veeeery slow flickering frequency.
        )
        {
            l.flickeringRadius = l.flickeringNextRadius;
            float minTimeRemaining = deltaTime + l.radiusFlickeringFrequency * .1;
            l.flickeringTimeRemaining = mu::random(minTimeRemaining, max<float>(minTimeRemaining, 1. / l.radiusFlickeringFrequency));
            l.flickeringNextRadius = mu::random(l.radiusFlickeringIntensity);
        }
        float x = deltaTime / l.flickeringTimeRemaining;
        float radiusDiff = l.flickeringNextRadius - l.flickeringRadius;
        l.flickeringRadius += radiusDiff * x;
    });
}
