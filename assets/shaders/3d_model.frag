precision mediump float;

layout(location = 0) out uint indexedColor;

uniform uint colorIndex;

void main()
{
    indexedColor = colorIndex;
}
