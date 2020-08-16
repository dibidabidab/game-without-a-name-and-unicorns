#version 300 es
precision mediump float;
precision lowp sampler2DArray;

out vec3 color;

in vec2 v_texCoords;

uniform sampler2D rgbImage;
uniform sampler2D bloomImage;

void main()
{

    color = texture(rgbImage, v_texCoords).rgb;

    {   // VIGNETTE:
        vec2 uv = v_texCoords;
        uv *=  1.0 - uv.yx;
        float vig = uv.x * uv.y * 100.0;
        vig = pow(vig, .15);
        color *= min(1.f, vig);
    }
    {   // BLOOM:
        vec3 bloomValue = texture(bloomImage, v_texCoords).rgb;
        float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

        for(int i = 1; i < 5; ++i)
        {
            bloomValue += textureOffset(bloomImage, v_texCoords, ivec2(0, i)).rgb * weight[i];
            bloomValue += textureOffset(bloomImage, v_texCoords, ivec2(0, -i)).rgb * weight[i];
        }
        color += bloomValue;
    }
}
