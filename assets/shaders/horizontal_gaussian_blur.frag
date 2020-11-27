precision mediump float;

out vec3 color;

in vec2 v_texCoords;

uniform sampler2D bloomImage;

void main()
{
    float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

    float texOffset = .002f;

    color = texture(bloomImage, v_texCoords).rgb;

    // 1:
    color += textureOffset(bloomImage, v_texCoords, ivec2(1, 0)).rgb * weight[1];
    color += textureOffset(bloomImage, v_texCoords, ivec2(-1, 0)).rgb * weight[1];

    // 2:
    color += textureOffset(bloomImage, v_texCoords, ivec2(2, 0)).rgb * weight[2];
    color += textureOffset(bloomImage, v_texCoords, ivec2(-2, 0)).rgb * weight[2];

    // 3:
    color += textureOffset(bloomImage, v_texCoords, ivec2(3, 0)).rgb * weight[3];
    color += textureOffset(bloomImage, v_texCoords, ivec2(-3, 0)).rgb * weight[3];

    // 4:
    color += textureOffset(bloomImage, v_texCoords, ivec2(4, 0)).rgb * weight[4];
    color += textureOffset(bloomImage, v_texCoords, ivec2(-4, 0)).rgb * weight[4];
}
