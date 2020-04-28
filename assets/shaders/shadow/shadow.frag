#version 300 es
precision mediump float;
precision lowp usampler2D;

layout(location = 0) out uint shadow;

uniform usampler2D tileMapTexture;

in float v_depth;
in vec2 v_pos;

void main()
{
    shadow = 1u;
    gl_FragDepth = .1;

    if (v_depth < 10.)
    {
        uint tileMapPixel = texelFetch(tileMapTexture, ivec2(v_pos.x, v_pos.y), 0).r;

        if (tileMapPixel != 0u)
        {
            shadow = 0u;
            gl_FragDepth = .2;
        }
    }

}
