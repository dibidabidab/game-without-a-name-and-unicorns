#version 300 es

layout(location = 0) in int pointIndex;

// per instance:
layout(location = 1) in int bottomY;
layout(location = 2) in int leftX;
layout(location = 3) in int rightX;
layout(location = 4) in int topLeftY;
layout(location = 5) in int topRightY;
layout(location = 6) in int color;

uniform mat4 projection;

flat out uint colorIndex;
out vec2 coords;
out vec2 reflectedCoords;

void main()
{
    colorIndex = uint(color);
    vec3 pos = vec3(0, 0, -512);

    if (pointIndex == 0 || pointIndex == 1)
        pos.x = float(leftX);
    else
        pos.x = float(rightX);

    if (pointIndex == 0)
        pos.y = float(topLeftY - bottomY + topLeftY);
    else if (pointIndex == 3)
        pos.y = float(topRightY - bottomY + topRightY);
    else if (pointIndex == 1)
        pos.y = float(topLeftY);
    else
        pos.y = float(topRightY);

    gl_Position = projection * vec4(pos, 1);

    reflectedCoords = gl_Position.xy;
    reflectedCoords *= .5;
    reflectedCoords += .5;

    ////////////////

    if (pointIndex == 0 || pointIndex == 3)
        pos.y = float(bottomY);

    gl_Position = projection * vec4(pos, 1);

    coords = gl_Position.xy;
    coords *= .5;
    coords += .5;
}
