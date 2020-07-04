
#include "RainbowSystem.h"
#include "../components/combat/Arrow.h"

void RainbowSystem::updateRainbowSpawners(float deltaTime)
{
    room->entities.view<RainbowSpawner, AABB>().each([&](auto e, RainbowSpawner &spawner, const AABB &aabb) {

        if (spawner.rainbowEntity == entt::null)
        {
            spawner.rainbowEntity = room->getTemplate("Rainbow").create();
            room->entities.assign_or_replace<Rainbow>(spawner.rainbowEntity, spawner.rainbowSettings);
        }

        Polyline *rainbowLine = room->entities.try_get<Polyline>(spawner.rainbowEntity);
        AABB *rainbowAABB = room->entities.try_get<AABB>(spawner.rainbowEntity);
        Rainbow *rainbow = room->entities.try_get<Rainbow>(spawner.rainbowEntity);
        if (!rainbowLine || !rainbowAABB || !rainbow)
        {
            room->entities.remove<RainbowSpawner>(e);
            return;
        }

        if (rainbowLine->points.empty())
        {
            ivec2 offset(spawner.maxRainbowLength * .5, 32);
            rainbowAABB->center = aabb.center + offset;
        }
        ivec2 currPos = aabb.center - rainbowAABB->center;

        if (rainbowLine->points.empty())
        {
            rainbowLine->points.emplace_back(currPos);
            rainbowLine->points.emplace_back(currPos);
            spawner.prevPoint = currPos;
        }


        bool front = !rainbow->startDissapearingFromLeft;
        if (currPos.x != spawner.prevPoint.x)
        {
            front = currPos.x < spawner.prevPoint.x;
            rainbow->startDissapearingFromLeft = !front;
        }

        auto &toMove = front ? rainbowLine->points.front() : rainbowLine->points.back();

        toMove = currPos;

        float len = length(toMove - vec2(spawner.prevPoint));
        if (len > 10)
        {
            if (front)
                rainbowLine->points.emplace_front(currPos);
            else
                rainbowLine->points.emplace_back(currPos);
            spawner.prevPoint = currPos;
            spawner.currentLength += len;
        }
        if (spawner.currentLength >= spawner.maxRainbowLength)
            room->entities.remove<RainbowSpawner>(e);
    });

}

void RainbowSystem::updateDisappearingRainbows(float deltaTime)
{

    room->entities.view<Rainbow, Polyline, PolyPlatform>().each([&](auto e, Rainbow &rainbow, Polyline &line, PolyPlatform &platform) {

        rainbow.timer += deltaTime;

        // check if rainbow should disappear:

        if (rainbow.disappearAfterTime > 0 && rainbow.timer >= rainbow.disappearAfterTime)
            rainbow.disappearing = true;

        if (rainbow.disappearOnPlayerTouch)
            for (auto eOnPlatform : platform.entitiesOnPlatform)
                if (room->entities.try_get<PlayerControlled>(eOnPlatform) != NULL)
                {
                    Physics *p = room->entities.try_get<Physics>(eOnPlatform);

                    if (p != NULL && p->prevVelocity.y <= 0) // dont make rainbow disappear when player jumps through platform..
                        rainbow.disappearing = true;
                }


        if (!rainbow.disappearing)
            return;
        // rainbow should disappear:

        if (line.points.size() <= 1)
        {
            room->entities.destroy(e);
            return;
        }

        vec2 &p0 = *(rainbow.startDissapearingFromLeft ? line.points.begin() : std::prev(line.points.end(), 1));
        vec2 &p1 = *(rainbow.startDissapearingFromLeft ? std::next(line.points.begin()) : std::prev(line.points.end(), 2));

        vec2 diff = p1 - p0;
        float len = length(diff);
        vec2 dir = diff / len;

        p0 += dir * std::min<float>(len, rainbow.disappearSpeed * deltaTime);
        if (len < 1)
        {
            if (rainbow.startDissapearingFromLeft)
            {
                line.points.pop_front();

                PolyPlatformWaves *waves = room->entities.try_get<PolyPlatformWaves>(e);
                if (waves && !waves->springs.empty())
                    waves->springs.erase(waves->springs.begin());
            }
            else
                line.points.pop_back();
        }
    });
}

void RainbowSystem::limitRainbowArrowAngle()
{
    room->entities.view<Arrow, RainbowSpawner, Physics>().each([&](Arrow &arr, RainbowSpawner &spawner, Physics &physics) {

        if (physics.velocity.x == 0)
            physics.velocity.x = 1; // prevent division by 0

        float steepness = abs(physics.velocity.y) / abs(physics.velocity.x);

        auto prevVel = physics.velocity;

        if (steepness > 1)
        {
            float velTotal = length(physics.velocity);

            physics.velocity = velTotal * normalize(vec2(physics.velocity.x, physics.velocity.y / steepness));
        }
    });
}
