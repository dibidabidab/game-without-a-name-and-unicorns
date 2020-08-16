#version 300 es
precision mediump float;
precision lowp usampler2D;
precision lowp sampler2DArray;

layout(location=0) out vec3 color;
layout(location=1) out vec3 bloomColor;

in vec2 v_texCoords;

uniform usampler2D indexedImage;
uniform usampler2D lightMap;
uniform usampler2D reflectionsMap;

uniform sampler2DArray palettes;
uniform uint paletteEffect;
uniform uint prevPaletteEffect;
uniform float timeSinceNewPaletteEffect;

uniform ivec2 realResolution;

#define PALETTE_TRANSITION_DURATION .3

vec3 getColorFromPalette(uint indexedColor, uint lightLevel, uint paletteEffect)
{
    vec3 colorFromPalette = texelFetch(palettes, ivec3(indexedColor, lightLevel, paletteEffect), 0).rgb;
    if (timeSinceNewPaletteEffect < PALETTE_TRANSITION_DURATION)
    {
        vec3 prevColorFromPalette = texelFetch(palettes, ivec3(indexedColor, lightLevel, prevPaletteEffect), 0).rgb;
        float progress = timeSinceNewPaletteEffect / PALETTE_TRANSITION_DURATION;
        colorFromPalette = mix(prevColorFromPalette, colorFromPalette, progress);
    }
    return colorFromPalette;
}

void main()
{
    ivec2 pixelCoords = ivec2(v_texCoords * vec2(realResolution) / vec2(3.));

    uint indexedColor = texelFetch(indexedImage, pixelCoords, 0).r;

    if (indexedColor == 0u)
        indexedColor = 2u; // 2 == sky

    uint reflectionColor = texelFetch(reflectionsMap, pixelCoords, 0).r;
//    if (reflectionColor != 0u)
//        indexedColor = reflectionColor;

    uint lightLevel = texelFetch(lightMap, pixelCoords, 0).r;

    vec3 colorFromPalette = getColorFromPalette(indexedColor, lightLevel, paletteEffect);
    color = colorFromPalette;

    if (reflectionColor != 0u)
    {
        vec3 reflectionColorFromPalette = getColorFromPalette(reflectionColor, lightLevel, paletteEffect);
        color.rgb *= .75;
        color.rgb += reflectionColorFromPalette * .25   ;
    }

    if (lightLevel == 2u)
        bloomColor = getColorFromPalette(indexedColor, 3u, paletteEffect);
    else
        bloomColor = vec3(0.);
}
