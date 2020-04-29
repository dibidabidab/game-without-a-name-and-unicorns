#version 300 es
precision mediump float;
precision lowp usampler2D;

layout(location = 0) out uint lightLevel;

uniform usampler2D shadowTexture;

flat in float v_radius;
flat in ivec2 v_shadowTexturePos;
in vec2 v_pos;

void main()
{
    float dist = length(v_pos);

    if (dist > 1.)
        discard;

    lightLevel = dist > .9 ? 1u : 2u;

    if (v_shadowTexturePos.x == -1)
        return;

    ivec2 shadowTextureCoords = v_shadowTexturePos;
    shadowTextureCoords.x += int(v_pos.x * v_radius);
    shadowTextureCoords.y += int(v_pos.y * v_radius);
    uint shadowLevel = texelFetch(shadowTexture, shadowTextureCoords, 0).r;

    if (shadowLevel > lightLevel)
        lightLevel = 0u;
    else
        lightLevel -= shadowLevel;

    gl_FragDepth = 1. -.1 * float(lightLevel);
}
