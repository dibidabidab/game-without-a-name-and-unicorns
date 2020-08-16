#version 300 es
precision mediump float;
precision lowp sampler2DArray;

out vec3 color;

in vec2 v_texCoords;

uniform sampler2D bloomImage;

void main()
{
    float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

    float texOffset = .002f;

    color = texture(bloomImage, v_texCoords).rgb;

    for(int i = 1; i < 5; ++i)
    {
        color += textureOffset(bloomImage, v_texCoords, ivec2(i, 0)).rgb * weight[i];
        color += textureOffset(bloomImage, v_texCoords, ivec2(-i, 0)).rgb * weight[i];
    }
}
