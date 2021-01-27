layout(location = 0) in vec2 quad_pos;

// per instance:
layout(location = 1) in vec2 particle_pos;
layout(location = 2) in float age;

uniform mat4 projection;
uniform float time;

out vec2 v_quad_pos;
flat out float v_age;

void main()
{
    float xOffset = sin(age * 6.5) * 4.;

    v_age = age;
    v_quad_pos = quad_pos;
    gl_Position = projection * vec4(vec3(quad_pos * 100.f * (1. - age) + particle_pos + vec2(.5 + xOffset, .5), 0), 1);
}
