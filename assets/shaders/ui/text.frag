precision mediump float;
precision lowp usampler2D;

layout(location = 0) out uint indexedColor;

uniform usampler2D spriteSheet;

in vec2 v_texCoords;

void main()
{
    indexedColor = texelFetch(spriteSheet, ivec2(v_texCoords), 0).r;

    if (indexedColor == 0u) // transparent
        discard;
}
