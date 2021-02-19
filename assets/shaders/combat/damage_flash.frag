precision mediump float;

layout(location = 0) out uint indexedColor;
layout(location = 1) out uint lightLevel;

uniform float time;

in vec2 coords;

#define PI 3.14159265
#define TAU (2. * PI)

void main()
{
    float angle = atan(coords.y, coords.x);

    float spikeFreq = 2.5;

    float spike = fract(angle * spikeFreq);
    spike = spike < .5 ? spike : 1. - spike;
    spike *= 2.;
    spike *= spike;


    float spikeTime = max(0., (time - .2) * 4.);

    float spikeDist = min(spikeTime, 1. - time) * .4 + spike * .5 * min(spikeTime, 1. - time * .5);

    float dist = length(coords);

    if (dist < .2 + time * .5 && dist > -1.5 + time * 3.)
    {
        indexedColor = 7u;
        lightLevel = 2u;
    }
    else if (dist < spikeDist)
    {
        uint colorA = 10u;
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
