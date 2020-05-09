#version 300 es

layout(location = 0) in vec2 a_pos;

// per instance:
layout(location = 1) in vec3 spritePos;
layout(location = 2) in vec2 spriteSize;
layout(location = 3) in vec2 textureOffset;

uniform mat4 projection;

out vec2 v_texCoords;

void main()
{
    v_texCoords = vec2(a_pos.x, 1. - a_pos.y) * spriteSize + textureOffset;

    gl_Position = projection * vec4(vec3(a_pos * spriteSize, 0) + spritePos, 1);
}
