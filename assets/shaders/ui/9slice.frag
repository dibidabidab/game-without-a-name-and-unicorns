precision mediump float;
precision lowp usampler2D;

layout(location = 0) out uint indexedColor;

uniform usampler2D spriteSheet;

flat in uvec2 v_size;
flat in uvec2 v_sliceSize;
flat in uvec2 v_textureOffset;
flat in uvec2 v_topLeftOffset;
flat in uvec2 v_innerSize;

in vec2 v_coords;

/**
*
*   WARNING: I wrote this while I was extremely tired.
*   yes, that is a very bad excuse for this horrible code.
*
*   UPDATE: yeah i found a bug when the 9slice in aseprite is not in center. // todo
*
**/

void main()
{
    uvec2 coordsBL = uvec2(v_coords); // origin at BottomLeft
    uvec2 coordsTL = coordsBL;          // origin at TopLeft
    coordsTL.y = v_size.y - coordsTL.y - 1u;

    uvec2 textureCoords = uvec2(v_textureOffset);

    uint bottomHeight = v_sliceSize.y - v_innerSize.y - v_topLeftOffset.y + 1u;
    uint rightEdgeWidth = v_sliceSize.x - v_innerSize.x - v_topLeftOffset.x;

    if (coordsTL.x < v_topLeftOffset.x)
    {
        if (coordsTL.y < v_topLeftOffset.y)
        {
            // top left corner

            textureCoords += coordsTL;
        }
        else if (coordsTL.y > v_size.y - bottomHeight)
        {
            // bottom left corner

            textureCoords.y += v_sliceSize.y - 1u;
            textureCoords.y -= coordsBL.y;
            textureCoords.x += coordsBL.x;
        }
        else
        {
            textureCoords.x += coordsTL.x;
            textureCoords.y += v_topLeftOffset.y;

            uint y = coordsTL.y - v_topLeftOffset.y;
            y = y % v_innerSize.y;
            textureCoords.y += y;
        }
    }
    else if (coordsTL.x >= v_size.x - rightEdgeWidth)
    {
        if (coordsTL.y < v_topLeftOffset.y)
        {
            // top right corner
            textureCoords.x += v_sliceSize.x - rightEdgeWidth;
            textureCoords.x += coordsTL.x - (v_size.x - rightEdgeWidth);
            textureCoords.y += coordsTL.y;
        }
        else if (coordsTL.y > v_size.y - bottomHeight)
        {
            // bottom right corner

            textureCoords.x += v_sliceSize.x - rightEdgeWidth;
            textureCoords.x += coordsTL.x - (v_size.x - rightEdgeWidth);
            textureCoords.y += v_sliceSize.y - coordsBL.y - 1u;
        }
        else
        {
            // right edge
            textureCoords.x += v_sliceSize.x;
            textureCoords.x -= v_size.x - coordsTL.x;
            textureCoords.y += v_topLeftOffset.y;

            uint y = coordsTL.y - v_topLeftOffset.y;
            y = y % v_innerSize.y;
            textureCoords.y += y;
        }
    }
    else
    {

        // middle
        if (coordsTL.y < v_topLeftOffset.y)
        {
            // top edge

            textureCoords.x += v_topLeftOffset.x;
            textureCoords.y += coordsTL.y;

            uint x = coordsTL.x - v_topLeftOffset.x;
            x = x % v_innerSize.x;
            textureCoords.x += x;
        }
        else if (coordsTL.y > v_size.y - bottomHeight)
        {
            // bottom edge

            textureCoords.y += v_sliceSize.y - 1u;
            textureCoords.y -= coordsBL.y;

            textureCoords.x += v_topLeftOffset.x;
            uint x = coordsTL.x - v_topLeftOffset.x;
            x = x % v_innerSize.x;
            textureCoords.x += x;
        }
        else
        {
            // middle middle

            textureCoords += v_topLeftOffset;
            uint x = coordsTL.x - v_topLeftOffset.x;
            x = x % v_innerSize.x;
            textureCoords.x += x;

            uint y = coordsTL.y - v_topLeftOffset.y;
            y = y % v_innerSize.y;
            textureCoords.y += y;
        }
    }

    indexedColor = texelFetch(spriteSheet, ivec2(textureCoords), 0).r;

    if (indexedColor == 0u)
        discard;
}
