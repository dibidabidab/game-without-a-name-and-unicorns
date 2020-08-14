#version 300 es

layout(location = 0) in int pointIndex;

// per instance:
layout(location = 1) in int bottomY;
layout(location = 2) in int leftX;
layout(location = 3) in int rightX;
layout(location = 4) in float topLeftY;
layout(location = 5) in float topRightY;
layout(location = 6) in int color;

uniform mat4 projection;

flat out uint colorIndex;
out vec2 projectedCoords;
out vec2 worldCoords;
out vec2 reflectedWorldCoords;

void main()
{
    colorIndex = uint(color);
    vec3 pos = vec3(0, 0, -512);

    if (pointIndex == 0 || pointIndex == 1)
        pos.x = float(leftX);
    else
        pos.x = float(rightX);

    if (pointIndex == 0)
        pos.y = topLeftY - float(bottomY) + topLeftY;
    else if (pointIndex == 3)
        pos.y = topRightY - float(bottomY) + topRightY;
    else if (pointIndex == 1)
        pos.y = topLeftY;
    else
        pos.y = topRightY;

    reflectedWorldCoords = pos.xy;

    ////////////////

    if (pointIndex == 0 || pointIndex == 3)
        pos.y = float(bottomY);

    gl_Position = projection * vec4(pos, 1);

    projectedCoords = gl_Position.xy;
    projectedCoords *= .5;
    projectedCoords += .5;

    worldCoords = pos.xy;
}
