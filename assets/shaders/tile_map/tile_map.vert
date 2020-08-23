layout(location = 0) in vec3 a_pos;

out vec2 v_texCoords;

uniform vec2 camPos;
uniform vec2 camSize;

void main()
{
    v_texCoords = a_pos.xy * (camSize * .5) + camPos;

    gl_Position = vec4(a_pos, 1);
}
