precision mediump float;
precision lowp usampler2D;


layout(location = 0) out uint indexedColor;

in vec2 v_texCoords;

uniform usampler2D mapTexture;
uniform usampler2D bloodSplatterTexture;


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
    }
}
