//
// Created by sneeuwpop on 01-06-20.
//

#ifndef GAME_TEXTGENERATORSYSTEM_H
#define GAME_TEXTGENERATORSYSTEM_H

#include "../EntitySystem.h"
#include "../../components/generators/TextGenerator.h"
#include "../../components/physics/Physics.h"

class TextGeneratorSystem : public EntitySystem
{
    using EntitySystem::EntitySystem;
protected:

    void update(double deltaTime, Room *room) override
    {
        room->entities.view<TextGenerator, AABB>().each([&](auto strEntity, TextGenerator &str, AABB &aabb) {
            if (str.getHash() != str.oldHash)
            {
                int i = 0;
                int x = aabb.center.x;
                int y = aabb.center.y;

                for (char& c : str.string)
                {
                    entt::entity e;
                    if (str.children.size() <= i)
                        str.children.push_back(room->entities.create());
                    e = str.children.at(i);

                    room->entities.get_or_assign<Child>(e, strEntity, std::to_string(x));
                    room->entities.get_or_assign<AABB>(e, ivec2(4, 8), ivec2(x,y));
                    room->entities.get_or_assign<AsepriteView>(e, asset<aseprite::Sprite>("sprites/menu/font"), (int)c);

                    x += 8;
                    i++;
                }

                while (i < str.children.size() ) { // make the cache of additional chars hidden
                    auto e = str.children.at(i);
                    room->entities.remove_if_exists<AsepriteView>(e);
                    i++;
                }

                str.oldHash = str.getHash();
            }
        });
    }

};

#endif //GAME_TEXTGENERATORSYSTEM_H
