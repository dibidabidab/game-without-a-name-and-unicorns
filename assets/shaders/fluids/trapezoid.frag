layout(location = 0) out uint indexedColor;

flat in uint colorIndex;

void main()
{
    if (colorIndex == 0u) // transparent
        discard;

    indexedColor = colorIndex;
}
