#version 300 es
precision mediump float;

out vec4 color;
in float v_depth;

void main()
{
    color = vec4(v_depth < 16., 0, 0, 1);
}
