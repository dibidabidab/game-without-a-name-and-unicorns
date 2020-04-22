#version 300 es
precision mediump float;
precision lowp usampler2D;
precision lowp sampler3D;

out vec3 color;
in vec2 v_texCoords;

uniform usampler2D indexedImage;

uniform sampler3D palettes;

void main()
{
    uint indexedColor = texture(indexedImage, v_texCoords).r;

    if (indexedColor == 0u)
        discard;

    color = vec3(float(indexedColor) / 4., 0, 0);
}
