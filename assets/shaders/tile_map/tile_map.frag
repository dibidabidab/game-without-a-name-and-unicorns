#version 300 es
precision mediump float;
precision lowp usampler2D;


layout(location = 0) out uint indexedColor;

in vec2 v_texCoords;

uniform usampler2D mapTexture;


void main()
{
    if (v_texCoords.x < 0. || v_texCoords.x > 1. || v_texCoords.y < 0. || v_texCoords.y > 1.)
    {
        indexedColor = 1u;
        return;
    }

    indexedColor = texture(mapTexture, v_texCoords).r;
    if (indexedColor == 0u)
        discard;
}
