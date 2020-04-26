#version 300 es

layout(location = 0) in vec2 a_pos;
layout(location = 1) in float a_depth;

out float v_depth;

uniform mat4 projection;

void main()
{
    v_depth = a_depth;
    gl_Position = projection * vec4(a_pos.x, a_pos.y, 0, 1);
}
