//
// Created by sneeuwpop on 01-06-20.
//

#ifndef GAME_TEXTGENERATOR_H
#define GAME_TEXTGENERATOR_H

#include "../../../../macro_magic/component.h"

COMPONENT(
        TextGenerator,
        HASH(string),
        FIELD_DEF_VAL(std::string, string, "")
)
size_t oldHash = 0; // FIXME: this should be a general
std::vector<entt::entity> children;

END_COMPONENT(TextGenerator)

#endif //GAME_TEXT_H
