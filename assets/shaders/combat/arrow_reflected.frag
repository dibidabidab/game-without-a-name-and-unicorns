precision mediump float;

layout(location = 0) out uint indexedColor;
layout(location = 1) out uint lightLevel;

uniform float attackAngle;
uniform float time;

in vec2 coords;

#define PI 3.14159265
#define TAU (2. * PI)

void main()
{

    float angle = atan(coords.y, coords.x);
    float angleDiff = min(abs(angle - attackAngle), min(abs((angle - TAU) - attackAngle), abs((attackAngle - TAU) - angle)));

    float spikeFreq = 2.5;

    float spike = fract(angle * spikeFreq);
    spike = spike < .5 ? spike : 1. - spike;
    spike *= 2.;
    spike *= spike;

    spike = max(0., spike - angleDiff * .4) * min(time * 4., 1.) * max(0., 1. - (time + .3) * time * 4.);

    float spikeDist = .5 + spike * 1.;

    float dist = length(coords);

    if (dist < spikeDist && dist > min(.2 + angleDiff * .3 * time + time, .5))
    {
        uint colorA = 12u;
        uint colorB = 7u;

        if (mod(time, .2) < .1)
        {
            uint tmp = colorA;
            colorA = colorB;
            colorB = tmp;
        }

        if (mod(time, .1) < .05)
            colorA = colorB;

        indexedColor = fract(angle * spikeFreq * .5) > .5 ? colorA : colorB;

        lightLevel = 2u;
    }
    else discard;
}
