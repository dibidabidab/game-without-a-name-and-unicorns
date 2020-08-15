#version 300 es

precision mediump float;
precision lowp usampler2D;

layout(location = 0) out uint reflectionColor;

flat in uint colorIndex;
in vec2 projectedCoords;
in vec2 reflectedWorldCoords;
in vec2 worldCoords;

uniform mat4 projection;
uniform usampler2D indexedImage;
uniform sampler2D indexedImageDepth;
uniform float time;

#define Z_NEAR .1
#define Z_FAR 1000.

float linearDepth(float depthSample)
{
    depthSample = 2.0 * depthSample - 1.0;
    float zLinear = 2.0 * Z_NEAR * Z_FAR / (Z_FAR + Z_NEAR - depthSample * (Z_FAR - Z_NEAR));
    return zLinear;
}

void main()
{
    uint indexedColor = texture(indexedImage, projectedCoords).r;

    if (indexedColor != colorIndex)
        discard;

    vec2 distortedReflectedWorldCoords = reflectedWorldCoords;
    distortedReflectedWorldCoords.x += sin(worldCoords.y * .7 + time * 8.) * 2.;

    vec2 projectedDistortedReflectedCoords = (projection * vec4(distortedReflectedWorldCoords, 0, 1.)).xy;
    projectedDistortedReflectedCoords *= .5;
    projectedDistortedReflectedCoords += .5;

    if (projectedDistortedReflectedCoords.y > 1.)
        discard;

    uint originalReflectionColor = texture(indexedImage, projectedDistortedReflectedCoords).r;

    if (originalReflectionColor == 0u)
        discard;

    float depth = texture(indexedImageDepth, projectedDistortedReflectedCoords).r;

    if (depth > .5)
        discard;

    reflectionColor = originalReflectionColor;//colorIndex + 1u;
}
