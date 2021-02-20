layout(location = 0) out uint indexedColor;

flat in int colorIndex;

void main()
{
    if (colorIndex == 0) // transparent
        discard;

    indexedColor = uint(abs(colorIndex));
}
