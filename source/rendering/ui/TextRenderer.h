
#ifndef GAME_TEXTRENDERER_H
#define GAME_TEXTRENDERER_H


#include "../../ecs/components/ui/UIComponents.h"

class TextRenderer
{

  public:

    void add(const TextView &, const ivec2 &screenPos);

    void render();

};


#endif
