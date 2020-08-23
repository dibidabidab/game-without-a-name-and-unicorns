layout(location = 0) in vec2 a_pos;

// per instance:
layout(location = 1) in vec3 spritePos;
layout(location = 2) in vec2 spriteSize;
layout(location = 3) in vec2 textureOffset;
layout(location = 4) in int rotate90Degs;

layout(location = 5) in int clipLowerLeftX;
layout(location = 6) in int clipLowerLeftY;

layout(location = 7) in int clipLowerRightX;
layout(location = 8) in int clipLowerRightY;

layout(location = 9) in int clipTopLeftX;
layout(location = 10) in int clipTopLeftY;

layout(location = 11) in int clipTopRightX;
layout(location = 12) in int clipTopRightY;

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
    ivec2 offset = ivec2(0);

    if (a_pos.x == 0.f) // left
    {
        if (a_pos.y == 0.f) // lower left
        {
            offset.x += clipLowerLeftX;
            offset.y += clipLowerLeftY;
        }
        else    // top left
        {
            offset.x += clipTopLeftX;
            offset.y -= clipTopLeftY;
        }
    }
    else    // right
    {
        if (a_pos.y == 0.f) // lower right
        {
            offset.x -= clipLowerRightX;
            offset.y += clipLowerRightY;
        }
        else    // top right
        {
            offset.x -= clipTopRightX;
            offset.y -= clipTopRightY;
        }
    }

    v_texCoords = vec2(posRotated.x, 1. - posRotated.y) * spriteSize + textureOffset + vec2(offset.x, -offset.y);


    gl_Position = projection * vec4(vec3(a_pos * abs(spriteSize) + vec2(offset), 0) + spritePos, 1);
}
