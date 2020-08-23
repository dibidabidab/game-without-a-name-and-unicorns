layout(location = 0) in vec2 a_pos;

// per instance:
layout(location = 1) in vec2 lightPos;
layout(location = 2) in float radius;
layout(location = 3) in ivec2 shadowTexturePos;

uniform mat4 projection;

out vec2 v_pos;
flat out float v_radius;
flat out ivec2 v_shadowTexturePos;

void main()
{
    v_radius = radius;
    v_shadowTexturePos = shadowTexturePos;
    v_pos = a_pos;
    gl_Position = projection * vec4(a_pos.x * radius + lightPos.x, a_pos.y * radius + lightPos.y, 0, 1);
}
