precision mediump float;
precision lowp usampler2D;
precision lowp sampler2DArray;

layout(location=0) out vec3 color;
layout(location=1) out vec3 bloomColor;

in vec2 v_texCoords;

uniform usampler2D indexedImage;
uniform usampler2D lightMap;

#ifdef WATER_REFLECTIONS
uniform usampler2D reflectionsMap;
#endif

#ifdef FIRE_HEAT_DISTORTION
uniform sampler2D fireHeatMap;
#endif

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

float map(float value, float min1, float max1, float min2, float max2) {
    return min2 + (value - min1) * (max2 - min2) / (max1 - min1);
}

void main()
{
    ivec2 pixelCoords = ivec2(v_texCoords * vec2(realResolution) / vec2(PIXEL_SCALING));

    #ifdef FIRE_HEAT_DISTORTION

    #define MAX_DISTORT_HEAT .4

    float heat = texture(fireHeatMap, v_texCoords).r;
    if (heat > 0. && heat <= MAX_DISTORT_HEAT)
    {
        float heat01 = map(heat, 0., MAX_DISTORT_HEAT, 0., 1.);

        pixelCoords.x += int(min(heat01 * 10., 1.) * sin(heat01 * 3. + timeSinceNewPaletteEffect * 12. + v_texCoords.x * 200.) * 1.1);
        pixelCoords.y += int(min(heat01 * 10., 1.) * sin(heat01 * 3. + timeSinceNewPaletteEffect * 5. + v_texCoords.y * 100.) * 1.1);
    }
    #endif

    uint indexedColor = texelFetch(indexedImage, pixelCoords, 0).r;

    if (indexedColor == 0u)
        indexedColor = 2u; // 2 == sky

    uint lightLevel = texelFetch(lightMap, pixelCoords, 0).r;

    vec3 colorFromPalette = getColorFromPalette(indexedColor, lightLevel, paletteEffect);
    color = colorFromPalette;


    #ifdef WATER_REFLECTIONS
    uint reflectionColor = texelFetch(reflectionsMap, pixelCoords, 0).r;

    if (reflectionColor != 0u)
    {
        vec3 reflectionColorFromPalette = getColorFromPalette(reflectionColor, lightLevel, paletteEffect);
        color.rgb *= .75;
        color.rgb += reflectionColorFromPalette * .25   ;
    }
    #endif

    bloomColor = vec3(0.);
    #ifdef BLOOM
    if (lightLevel == 2u)
        bloomColor = getColorFromPalette(indexedColor, 3u, paletteEffect);
    #endif
}
