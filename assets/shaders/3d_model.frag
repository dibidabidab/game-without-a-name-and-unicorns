precision mediump float;

layout(location = 0) out uint indexedColor;
layout(location = 1) out uint lightLevel;

in vec3 normal;

uniform uint colorIndex;

void main()
{
    indexedColor = colorIndex;

    vec3 sunDir = normalize(vec3(.6, .3, -.1));

    float ldot = dot(normal, sunDir);
    lightLevel = ldot < 0.f ? 0u : (ldot > .5f ? 2u : 1u);
}
