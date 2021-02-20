
#include <ecs/EntityEngine.h>
#include "PositionedAudioSystem.h"
#include "../../generated/Physics.hpp"
#include "../../generated/Audio.hpp"
#include "../../tiled_room/TiledRoom.h"
#include <generated/SoundSpeaker.hpp>

void PositionedAudioSystem::update(double deltaTime, EntityEngine *engine)
{
    updateFrequency = 30;
    auto *room = dynamic_cast<TiledRoom *>(engine);
    if (!room)
        return;

    engine->entities.view<AABB, PositionedAudio, SoundSpeaker>().each([&](AABB &aabb, PositionedAudio &posAud, SoundSpeaker &speaker) {

        if (!speaker.source)
            return;

        vec2 diff = (vec2(aabb.center) - room->cameraPosition) / (16.f * 128.f);

        diff *= posAud.positionMultiplier;

        speaker.source->setPosition(vec3(diff, .5));
    });
}
