layout(location = 0) in vec3 a_pos;

uniform mat4 projection;
uniform uint radius;
uniform ivec2 position;

out vec2 coords;

void main()
{
    coords = a_pos.xy;
    vec2 pos = vec2(position) + (a_pos.xy * vec2(radius) * 2.);

    gl_Position = projection * vec4(vec3(pos, .5), 1);
}
