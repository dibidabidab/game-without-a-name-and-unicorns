layout(location = 0) in vec3 a_pos;

// per instance:
layout(location = 1) in int pointAX;
layout(location = 2) in int pointAY;
layout(location = 3) in int pointAZ;
layout(location = 4) in int pointBX;
layout(location = 5) in int pointBY;
layout(location = 6) in int pointBZ;
layout(location = 7) in uint a_colorIndex;

uniform mat4 projection;
uniform float halfWidth;

flat out uint colorIndex;

void main()
{
    colorIndex = a_colorIndex;
    vec3 pos = vec3(0, 0, 0);

    vec2 dir = normalize(vec2(pointBX, pointBY) - vec2(pointAX, pointAY));
    vec3 offset = vec3(vec2(-dir.y, dir.x) * halfWidth, 0);

    if (a_pos.x == 0.) // point A
        pos = vec3(pointAX, pointAY, pointAZ);
    else    // point B
        pos = vec3(pointBX, pointBY, pointBZ);

    if (a_pos.y == 0.)
        pos += offset;
    else pos -= offset;

    gl_Position = projection * vec4(pos, 1);
}
