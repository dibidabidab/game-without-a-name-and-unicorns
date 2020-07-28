#version 300 es

layout(location = 0) in vec2 quad_pos;

// per instance:
layout(location = 1) in vec2 drop_pos;
layout(location = 2) in float size;
layout(location = 3) in uint color;
layout(location = 4) in uint permanent;

uniform mat4 projection;
uniform uint drawPermanent;

out vec2 v_quad_pos;
flat out uint v_color;
flat out uint do_discard;

void main()
{
    do_discard = drawPermanent == permanent ? 0u : 1u;
    v_quad_pos = quad_pos;
    v_color = color;
    gl_Position = projection * vec4(vec3(quad_pos * size + drop_pos + vec2(.5), 0), 1);
}
