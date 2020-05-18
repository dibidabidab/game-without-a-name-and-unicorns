#version 300 es

layout(location = 0) in vec2 a_pos;

// per instance:
layout(location = 1) in vec3 spritePos;
layout(location = 2) in vec2 spriteSize;
layout(location = 3) in vec2 textureOffset;
layout(location = 4) in int rotate90Degs;

uniform mat4 projection;

out vec2 v_texCoords;

void main()
{
    vec2 posRotated = a_pos;
    if (rotate90Degs != 0)
    {
        posRotated.x = a_pos.y;
        posRotated.y = a_pos.x;
    }
    v_texCoords = vec2(posRotated.x, 1. - posRotated.y) * spriteSize + textureOffset;

    gl_Position = projection * vec4(vec3(a_pos * abs(spriteSize), 0) + spritePos, 1);
}
