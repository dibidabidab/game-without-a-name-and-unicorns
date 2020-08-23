layout(location = 0) in vec2 a_pos;
layout(location = 1) in float a_depth;

uniform mat4 projection;

out float v_depth;
out vec2 v_pos;

void main()
{
    v_pos = a_pos;
    v_depth = a_depth;
    gl_Position = projection * vec4(a_pos.x, a_pos.y, 0, 1);
}
