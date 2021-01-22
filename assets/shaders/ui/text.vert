layout(location = 0) in vec2 a_pos;

// per instance:
layout(location = 1) in ivec3 charScreenPos;
layout(location = 2) in uvec2 spriteOffset;
layout(location = 3) in uvec2 size;
layout(location = 4) in uint mapColorFrom;
layout(location = 5) in uint mapColorTo;

uniform mat4 projection;

out vec2 v_texCoords;
flat out uint v_mapColorFrom;
flat out uint v_mapColorTo;

void main()
{
    v_mapColorFrom = mapColorFrom;
    v_mapColorTo = mapColorTo - 1u;
    v_texCoords = vec2(size) * a_pos;

    vec2 pos = vec2(charScreenPos.xy) + v_texCoords;

    v_texCoords.y = float(size.y) - v_texCoords.y;

    v_texCoords += vec2(spriteOffset);

    gl_Position = projection * vec4(vec3(pos, charScreenPos.z), 1);
}
