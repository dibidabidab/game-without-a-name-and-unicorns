precision mediump float;

layout(location = 0) out uint lightLevel;

void main()
{
    lightLevel = 2u;

    gl_FragDepth = 1. -.1 * float(lightLevel);
}
