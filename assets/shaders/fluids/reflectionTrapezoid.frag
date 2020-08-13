#version 300 es

precision mediump float;
precision lowp usampler2D;

layout(location = 0) out uint reflectionColor;

flat in uint colorIndex;
in vec2 reflectedCoords;
in vec2 coords;

uniform usampler2D indexedImage;

void main()
{

    uint indexedColor = texture(indexedImage, coords).r;

    if (indexedColor != colorIndex)
        discard;

    if (reflectedCoords.y > 1.)
        discard;

    reflectionColor = texture(indexedImage, reflectedCoords).r;

}
