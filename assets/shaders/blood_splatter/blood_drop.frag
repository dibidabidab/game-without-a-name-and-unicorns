#version 300 es
precision mediump float;
precision lowp usampler2D;

layout(location = 0) out uint indexedColor;

in vec2 v_quad_pos;
flat in uint v_color;
flat in uint do_discard;

void main()
{
    if (do_discard == 1u)
        discard;

    if (length(v_quad_pos) > 1.f)
        discard;
    indexedColor = v_color;
}
