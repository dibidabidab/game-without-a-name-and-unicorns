precision mediump float;

layout(location = 0) in int pointIndex;

// per instance:
layout(location = 1) in int color;
layout(location = 2) in int bottomY;
layout(location = 3) in int leftX;
layout(location = 4) in int rightX;
layout(location = 5) in float topLeftY;
layout(location = 6) in float topRightY;

uniform mat4 projection;

flat out int colorIndex_;
out vec2 projectedCoords;
out vec2 worldCoords;
out vec2 reflectedWorldCoords;

void main()
{
    colorIndex_ = color;
    vec3 pos = vec3(0, 0, -512);

    if (pointIndex == 0 || pointIndex == 1)
        pos.x = float(leftX);
    else
        pos.x = float(rightX);

    if (pointIndex == 0)
        pos.y = (topLeftY - float(bottomY) + topLeftY) * 1.;
    else if (pointIndex == 3)
        pos.y = (topRightY - float(bottomY) + topRightY) * 1.;
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
