layout(location = 0) in vec2 a_pos;

// per instance:
layout(location = 1) in ivec3 screenPos;
layout(location = 2) in ivec2 size;
layout(location = 3) in uvec2 sliceSize;
layout(location = 4) in uvec2 textureOffset;

layout(location = 5) in uvec2 topLeftOffset;
layout(location = 6) in uvec2 innerSize;

uniform mat4 projection;

flat out uvec2 v_size;
flat out uvec2 v_sliceSize;
flat out uvec2 v_textureOffset;
flat out uvec2 v_topLeftOffset;
flat out uvec2 v_innerSize;

out vec2 v_coords;

void main()
{
    v_coords = vec2(size) * a_pos;
    v_size = uvec2(size);
    v_sliceSize = sliceSize;
    v_textureOffset = textureOffset;
    v_topLeftOffset = topLeftOffset;
    v_innerSize = innerSize;

    vec3 vertPos = vec3(screenPos);
    vertPos.xy += v_coords;
    vertPos.y -= float(size.y);
    gl_Position = projection * vec4(vertPos, 1);
}
