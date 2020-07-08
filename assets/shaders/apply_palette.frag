#version 300 es
precision mediump float;
precision lowp usampler2D;
precision lowp sampler2DArray;

out vec4 color;
in vec2 v_texCoords;

uniform usampler2D indexedImage;
uniform usampler2D lightMap;

uniform sampler2DArray palettes;
uniform uint paletteEffect;
uniform uint prevPaletteEffect;
uniform float timeSinceNewPaletteEffect;

uniform ivec2 realResolution;

#define PALETTE_TRANSITION_DURATION .3

void main()
{
    ivec2 pixelCoords = ivec2(v_texCoords * vec2(realResolution) / vec2(3.));

    uint indexedColor = texelFetch(indexedImage, pixelCoords, 0).r;

    if (indexedColor == 0u)
        indexedColor = 2u; // 2 == sky

    uint lightLevel = texelFetch(lightMap, pixelCoords, 0).r;

    vec3 colorFromPalette = texelFetch(palettes, ivec3(indexedColor, lightLevel, paletteEffect), 0).rgb;

    if (timeSinceNewPaletteEffect < PALETTE_TRANSITION_DURATION)
    {
        vec3 prevColorFromPalette = texelFetch(palettes, ivec3(indexedColor, lightLevel, prevPaletteEffect), 0).rgb;
        float progress = timeSinceNewPaletteEffect / PALETTE_TRANSITION_DURATION;
        colorFromPalette = mix(prevColorFromPalette, colorFromPalette, progress);
    }

    color = vec4(colorFromPalette, 1);
}
