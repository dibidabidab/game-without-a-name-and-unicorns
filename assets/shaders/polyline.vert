#version 300 es

layout(location = 0) in int pointIndex;

// per instance:
layout(location = 1) in int pointAX;
layout(location = 2) in int pointAY;
layout(location = 3) in int pointBX;
layout(location = 4) in int pointBY;
layout(location = 5) in float zIndex;
layout(location = 6) in uint a_colorIndex;

uniform mat4 projection;

flat out uint colorIndex;

void main()
{
    colorIndex = a_colorIndex;
    vec3 pos = vec3(0, 0, zIndex);

    if (pointIndex == 0)
    {
        pos.x = float(pointAX);
        pos.y = float(pointAY);
    }
    else
    {
        pos.x = float(pointBX);
        pos.y = float(pointBY);
    }

    gl_Position = projection * vec4(pos, 1);
}
