#version 300 es

layout(location = 0) in int pointIndex;

// per instance:
layout(location = 1) in int pointAX;
layout(location = 2) in int pointAY;
layout(location = 3) in int pointAZ;
layout(location = 4) in int pointBX;
layout(location = 5) in int pointBY;
layout(location = 6) in int pointBZ;
layout(location = 7) in uint a_colorIndex;

uniform mat4 projection;

flat out uint colorIndex;

void main()
{
    colorIndex = a_colorIndex;
    vec3 pos = vec3(0, 0, 0);

    if (pointIndex == 0)
    {
        pos.x = float(pointAX);
        pos.y = float(pointAY);
        pos.z = float(pointAZ);
    }
    else
    {
        pos.x = float(pointBX);
        pos.y = float(pointBY);
        pos.z = float(pointBZ);
    }

    gl_Position = projection * vec4(pos, 1);
}
