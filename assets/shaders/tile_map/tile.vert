#version 300 es

layout(location = 0) in vec3 a_pos;

out vec2 v_texCoords;

uniform ivec2 mapSize;
uniform ivec2 tilePos;

void main()
{
    vec2 pos01 = a_pos.xy * vec2(.5) + vec2(.5);

    v_texCoords = pos01;
    v_texCoords.y = 1. - v_texCoords.y;

    vec2 drawPos = (vec2(tilePos) / vec2(mapSize)) * 2. - 1.;

    vec2 tileSize = vec2(2) / vec2(mapSize);

    drawPos += pos01 * (tileSize * 3.) - tileSize;

    gl_Position = vec4(drawPos.x, drawPos.y, 0, 1);
}
