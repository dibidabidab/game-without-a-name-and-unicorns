
#ifndef GAME_SOUNDSPEAKER_H
#define GAME_SOUNDSPEAKER_H

#include <audio/audio.h>
#include "../../../macro_magic/component.h"

COMPONENT(
    SoundSpeaker,
    HASH(sound.isSet(), volume, pitch, looping),
    FIELD(asset<au::Sound>, sound),
    FIELD_DEF_VAL(float, volume, 1.),
    FIELD_DEF_VAL(float, pitch, 1.),
    FIELD_DEF_VAL(bool, looping, false),
    FIELD_DEF_VAL(bool, paused, false)
)

    std::shared_ptr<au::SoundSource> source;

END_COMPONENT(SoundSpeaker)

#endif //GAME_SOUNDSPEAKER_H
