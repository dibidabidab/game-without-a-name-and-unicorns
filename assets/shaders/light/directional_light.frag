precision mediump float;

layout(location = 0) out uint lightLevel;

uniform sampler2D lightShaftsTexture;

in vec2 uv;

uniform float time;

void main()
{
    float textureVal = texture(lightShaftsTexture, vec2(uv.x + time * -.006, time * .004)).r * .8;
    textureVal += texture(lightShaftsTexture, vec2(uv.x + time * .01, time * -.002)).r * .3;
    if (textureVal > .6)
        lightLevel = 2u;
    else if (textureVal > .3)
        lightLevel = 1u;
    else
        lightLevel = 0u;

    gl_FragDepth = 1. -.1 * float(lightLevel);
}
