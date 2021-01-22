precision mediump float;
precision lowp usampler2D;

layout(location = 0) out uint indexedColor;

uniform usampler2D spriteSheet;

in vec2 v_texCoords;
flat in uint v_mapColorFrom;
flat in uint v_mapColorTo;

void main()
{
    indexedColor = texelFetch(spriteSheet, ivec2(v_texCoords), 0).r;

    if (indexedColor == v_mapColorFrom)
        indexedColor = v_mapColorTo + 1u;

    if (indexedColor == 0u) // transparent
        discard;
}
