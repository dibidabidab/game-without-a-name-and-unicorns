
layout(location = 0) in vec3 a_pos;
layout(location = 1) in vec3 a_normal;

uniform mat4 mvp;   // https://stackoverflow.com/questions/5550620/the-purpose-of-model-view-projection-matrix

out vec3 normal;

void main()
{

    normal = normalize(vec3(mvp * vec4(a_normal, 0)));
    gl_Position = mvp * vec4(a_pos, 1);
}
