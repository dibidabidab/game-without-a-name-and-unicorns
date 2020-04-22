#version 300 es
precision mediump float;
precision lowp usampler2D;

layout(location = 0) out uint indexedColor;

in vec2 v_texCoords;

uniform ivec2 tileTextureOffset;
uniform usampler2D tileSet;

void main()
{
    bool inner = true;

    if (v_texCoords.x < 1./3. || v_texCoords.x > 2./3. || v_texCoords.y < 1./3. || v_texCoords.y > 2./3.)
        inner = false;

    if (!inner)
        discard;

    ivec2 pixelPos = ivec2(v_texCoords * vec2(48)) + tileTextureOffset;

    uint pixel = texelFetch(tileSet, pixelPos, 0).r;

    if (pixel == 0u)
        discard;

    indexedColor = pixel;
//    color = vec4(float(pixel) / 4., 0, 0, 1);
}
