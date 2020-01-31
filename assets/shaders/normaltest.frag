#version 300 es
precision mediump float;

out vec4 color;
in vec3 v_normal;

void main()
{
    color = vec4(v_normal, 1);
}
