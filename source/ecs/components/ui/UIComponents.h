
#ifndef GAME_UICOMPONENTS_H
#define GAME_UICOMPONENTS_H

#include <utils/aseprite/Aseprite.h>
#include "../../../macro_magic/component.h"


COMPONENT(
    UIContainer,
    HASH(0),
    FIELD(asset<aseprite::Sprite>, nineSliceSprite),
    FIELD_DEF_VAL(bool, autoWidth, false),
    FIELD_DEF_VAL(bool, autoHeight, false),
    FIELD_DEF_VAL(int, fixedWidth, 128),
    FIELD_DEF_VAL(int, fixedHeight, 64),

    FIELD_DEF_VAL(ivec2, padding, ivec2(0)),

    FIELD_DEF_VAL(bool, centerAlign, false),

    FIELD_DEF_VAL(bool, fillRemainingParentHeight, false),
    FIELD_DEF_VAL(bool, fillRemainingParentWidth, false)
)

    ivec2 textCursor = ivec2(0), innerTopLeft = ivec2(0);
    int currentLineHeight = 0, minX = 0, maxX = 0, innerHeight = 0;

    const aseprite::Slice *spriteSlice = NULL;
    const aseprite::Slice::NineSlice *nineSlice = NULL;

    void resetCursorX()
    {
        if (centerAlign)
            textCursor.x = (minX + maxX) / 2;
        else
            textCursor.x = minX;
    }

    void goToNewLine(int spacing = 0)
    {
        resetCursorX();

        textCursor.y -= currentLineHeight + spacing;
        currentLineHeight = 0;
    }

    /**
     * if requireWidth fits in the current line:
     *      do nothing
     *
     * else if autoWidth is enabled:
     *      change width
     *
     * else:
     *      jump to next line (AND RETURN TRUE)
     */
    bool resizeOrNewLine(int requireWidth, int lineSpacing = 0)
    {
        if (centerAlign)
        {
            if (textCursor.x != (minX + maxX) / 2)
            {
                goToNewLine(lineSpacing);
                return true;
            }
            return false;
        }

        int maxXNeeded = textCursor.x + requireWidth;
        if (maxXNeeded <= maxX)
            return false;
        if (autoWidth)
            maxX = maxXNeeded;
        else
        {
            goToNewLine(lineSpacing);
            return true;
        }
        return false;
    }

    void resizeLineHeight(int minLineHeightRequired)
    {
        currentLineHeight = max(currentLineHeight, minLineHeightRequired);
    }

END_COMPONENT(UIContainer)


ENUM(HorizontalAlignment, left, center, right)
ENUM(VerticalAlignment, top, center, bottom)

COMPONENT(
        UIElement,
        HASH(0),
        FIELD_DEF_VAL(ivec2, margin, ivec2(0)),
        FIELD_DEF_VAL(bool, startOnNewLine, false),
        FIELD_DEF_VAL(int, lineSpacing, 3),

        FIELD_DEF_VAL(ivec2, renderOffset, ivec2(0)),

        FIELD_DEF_VAL(bool, absolutePositioning, false),
        FIELD_DEF_VAL(HorizontalAlignment, absoluteHorizontalAlign, HorizontalAlignment::left),
        FIELD_DEF_VAL(VerticalAlignment, absoluteVerticalAlign, VerticalAlignment::top)
)

    ivec2 getAbsolutePosition(const UIContainer &container, int width, int height)
    {
        ivec2 pos;
        switch (absoluteHorizontalAlign)
        {
            case HorizontalAlignment::left:
                pos.x = container.minX;
                break;
            case HorizontalAlignment::center:
                pos.x = (container.minX + container.maxX) / 2 - width / 2;
                break;
            case HorizontalAlignment::right:
                pos.x = container.maxX - width;
                break;
        }
        switch (absoluteVerticalAlign)
        {
            case VerticalAlignment::top:
                pos.y = container.innerTopLeft.y;
                break;
            case VerticalAlignment::center:
                pos.y = container.innerTopLeft.y - container.innerHeight / 2 + height / 2;
                break;
            case VerticalAlignment::bottom:
                pos.y = container.innerTopLeft.y - container.innerHeight + height;
                break;
        }
        return pos;
    }

END_COMPONENT(UIElement)


COMPONENT(
    TextView,
    HASH(0),
    FIELD(std::string, text),
    FIELD(asset<aseprite::Sprite>, fontSprite),
    FIELD_DEF_VAL(uint8, mapColorFrom, 0u),
    FIELD_DEF_VAL(uint8, mapColorTo, 0u),
    FIELD_DEF_VAL(int, letterSpacing, 1),

    FIELD_DEF_VAL(bool, waving, false),
    FIELD_DEF_VAL(float, wavingFrequency, 1.),
    FIELD_DEF_VAL(float, wavingSpeed, 1.),
    FIELD_DEF_VAL(float, wavingAmplitude, 8.)
)
END_COMPONENT(TextView)


#endif //GAME_UICOMPONENTS_H
