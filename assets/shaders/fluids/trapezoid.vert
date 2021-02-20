layout(location = 0) in int pointIndex;

// per instance:
layout(location = 1) in int color;
layout(location = 2) in int bottomY;
layout(location = 3) in int leftX;
layout(location = 4) in int rightX;
layout(location = 5) in float topLeftY;
layout(location = 6) in float topRightY;

uniform mat4 projection;

flat out int colorIndex;

void main()
{
    colorIndex = color;
    vec3 pos = vec3(0, 0, -512);

    if (pointIndex == 0 || pointIndex == 1)
        pos.x = float(leftX);
    else
        pos.x = float(rightX);

    if (pointIndex == 0 || pointIndex == 3)
        pos.y = float(bottomY);
    else if (pointIndex == 1)
        pos.y = topLeftY;
    else
        pos.y = topRightY;

    gl_Position = projection * vec4(pos, 1);
}
