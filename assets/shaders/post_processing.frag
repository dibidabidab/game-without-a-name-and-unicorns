precision mediump float;
precision lowp sampler2DArray;

out vec4 color;

in vec2 v_texCoords;

uniform sampler2D rgbImage;
uniform sampler2D bloomImage;

void main()
{

    color = texture(rgbImage, v_texCoords);

    #ifdef VIGNETTE
    {
        vec2 uv = v_texCoords;
        uv *=  1.0 - uv.yx;
        float vig = uv.x * uv.y * 100.0;
        vig = pow(vig, .15);
        color.rgb *= min(1.f, vig);
    }
    #endif

    #ifdef BLOOM
    {
        vec3 bloomValue = texture(bloomImage, v_texCoords).rgb;
        float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

        // 1:
        bloomValue += textureOffset(bloomImage, v_texCoords, ivec2(0, 1)).rgb * weight[1];
        bloomValue += textureOffset(bloomImage, v_texCoords, ivec2(0, -1)).rgb * weight[1];

        // 2:
        bloomValue += textureOffset(bloomImage, v_texCoords, ivec2(0, 2)).rgb * weight[2];
        bloomValue += textureOffset(bloomImage, v_texCoords, ivec2(0, -2)).rgb * weight[2];

        // 3:
        bloomValue += textureOffset(bloomImage, v_texCoords, ivec2(0, 3)).rgb * weight[3];
        bloomValue += textureOffset(bloomImage, v_texCoords, ivec2(0, -3)).rgb * weight[3];

        // 4:
        bloomValue += textureOffset(bloomImage, v_texCoords, ivec2(0, 4)).rgb * weight[4];
        bloomValue += textureOffset(bloomImage, v_texCoords, ivec2(0, -4)).rgb * weight[4];

        color.rgb += bloomValue;
    }
    #endif
}
