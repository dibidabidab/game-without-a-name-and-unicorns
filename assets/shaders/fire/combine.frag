precision mediump float;
precision lowp sampler2D;

layout(location = 0) out uint indexedColor;

uniform sampler2D particleMap;

in vec2 v_texCoords;

void main()
{
    float fireIntensity = texture(particleMap, v_texCoords).r;

    indexedColor = fireIntensity > .4 ? (fireIntensity == 1. ? 17u : 16u) : 0u;
    if (indexedColor == 0u)
        discard;
}
