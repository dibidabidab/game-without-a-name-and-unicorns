#version 300 es
precision mediump float;
precision lowp usampler2D;
precision lowp sampler2DArray;

out vec4 color;
in vec2 v_texCoords;

uniform usampler2D indexedImage;

uniform sampler2DArray palettes;

void main()
{
    uint indexedColor = texture(indexedImage, v_texCoords).r;

    if (indexedColor == 0u)
        discard;

    uint lightLevel = 0u;
    uint paletteEffect = 0u;

    vec3 colorFromPalette = texelFetch(palettes, ivec3(indexedColor, lightLevel, paletteEffect), 0).rgb;

    // useful for showing the palette fullscreen:
//    colorFromPalette = texelFetch(palettes, ivec3(int(v_texCoords.x * 8.), int(v_texCoords.y * 3.), 0), 0).rgb;

    color = vec4(colorFromPalette, 1);
//    color = vec4(float(indexedColor) / 4., 0, 0, 1);
}
