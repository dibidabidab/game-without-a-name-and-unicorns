precision mediump float;
precision lowp usampler2D;

layout(location = 0) out uint indexedColor;
layout(location = 1) out uint depth8Bit;

in vec2 v_texCoords;

uniform ivec2 tileTextureOffset;
uniform usampler2D tileSet;
uniform float zIndex;

float map(float value, float min1, float max1, float min2, float max2)
{
    return min2 + (value - min1) * (max2 - min2) / (max1 - min1);
}

void main()
{
    bool inner = true;

    if (v_texCoords.x < 1./3. || v_texCoords.x > 2./3. || v_texCoords.y < 1./3. || v_texCoords.y > 2./3.)
        inner = false;

    gl_FragDepth = (inner ? .5 : .4999) - zIndex * .002f;

//    if (!inner)
//        discard;

    ivec2 pixelPos = ivec2(v_texCoords * vec2(48)) + tileTextureOffset;

    indexedColor = texelFetch(tileSet, pixelPos, 0).r;

    if (indexedColor == 0u)
        discard;

    depth8Bit = uint(map(zIndex, 0.f, 100.f, 0.f, 255.f));
//    color = vec4(float(pixel) / 4., 0, 0, 1);
}
