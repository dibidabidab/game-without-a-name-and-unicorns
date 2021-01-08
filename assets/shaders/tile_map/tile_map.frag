precision mediump float;
precision lowp usampler2D;


layout(location = 0) out uint indexedColor;

in vec2 v_texCoords;

uniform usampler2D mapTexture;
uniform usampler2D depth8BitTexture;
uniform usampler2D bloodSplatterTexture;

float map(float value, float min1, float max1, float min2, float max2)
{
    return min2 + (value - min1) * (max2 - min2) / (max1 - min1);
}

void main()
{
    bool outsideRoom = v_texCoords.x < 0. || v_texCoords.x > 1. || v_texCoords.y < 0. || v_texCoords.y > 1.;

    if (outsideRoom)
        indexedColor = 1u;
    else
        indexedColor = texture(mapTexture, v_texCoords).r;

    if (indexedColor == 0u)
        discard;
    else if (!outsideRoom)
    {
        uint bloodColor = texture(bloodSplatterTexture, v_texCoords).r;
        if (bloodColor != 0u)
            indexedColor = bloodColor;

        float zIndex = map(float(texture(depth8BitTexture, v_texCoords).r), 0.f, 255.f, 1.f, 0.f);
        gl_FragDepth = zIndex;
    }
}
