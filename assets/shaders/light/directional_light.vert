layout(location = 0) in vec2 a_pos; // x = -1 or 1, y = -1 or 1

// per instance:
layout(location = 1) in vec2 lightPos;
layout(location = 2) in float width;
layout(location = 3) in float distance;
layout(location = 4) in float rotation;

uniform mat4 projection;

out vec2 uv;

vec2 rotate(vec2 v, float a) {
    float s = sin(a);
    float c = cos(a);
    mat2 m = mat2(c, s, -s, c);
    return m * v;
}

void main()
{
    uv = a_pos;
    uv *= .5;
    uv += .5;
    vec2 pos = a_pos;

    pos.x *= width * .5;
    if (pos.y > 0.)
        pos.y = 0.;
    pos.y *= distance;

    pos = rotate(pos, rotation);

    gl_Position = projection * vec4(pos + lightPos, 0, 1);
}
