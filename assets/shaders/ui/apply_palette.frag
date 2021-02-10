precision mediump float;
precision lowp usampler2D;
precision lowp sampler2DArray;

out vec4 color;

in vec2 v_texCoords;

uniform usampler2D indexedImage;

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
    ivec2 pixelCoords = ivec2(v_texCoords * vec2(realResolution) / vec2(PIXEL_SCALING));
    ivec2 maxCoords = ivec2(vec2(realResolution) / vec2(PIXEL_SCALING));

    uint indexedColor = texelFetch(indexedImage, pixelCoords, 0).r;

    if (pixelCoords.x >= maxCoords.x || pixelCoords.y >= maxCoords.y)
        indexedColor = 1u;

    if (indexedColor == 0u)
        discard;

    vec3 colorFromPalette = getColorFromPalette(indexedColor, 2u, paletteEffect);
    color = vec4(colorFromPalette, 1);
}
