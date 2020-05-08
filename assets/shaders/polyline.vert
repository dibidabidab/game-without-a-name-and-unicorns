#version 300 es

layout(location = 0) in int pointIndex;

// per instance:
layout(location = 1) in uint pointAX;
layout(location = 2) in uint pointAY;
layout(location = 3) in uint pointBX;
layout(location = 4) in uint pointBY;
layout(location = 5) in uint a_colorIndex;
layout(location = 6) in float zIndex;

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
