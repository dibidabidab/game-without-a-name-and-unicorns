
#ifndef GAME_CHILDRENSYSTEM_H
#define GAME_CHILDRENSYSTEM_H

#include "EntitySystem.h"
#include "../../room/Room.h"
#include "../components/Children.h"

class ChildrenSystem : public EntitySystem
{

    using EntitySystem::EntitySystem;

  protected:
    void init(Room *room) override
    {
        room->entities.on_construct<Child>().connect<&ChildrenSystem::onChildCreation>(this);
        room->entities.on_destroy<Child>().connect<&ChildrenSystem::onChildDeletion>(this);
        room->entities.on_destroy<Parent>().connect<&ChildrenSystem::onParentDeletion>(this);
    }

    void onChildCreation(entt::registry &reg, entt::entity entity)
    {
        Child &child = reg.get<Child>(entity);

        Parent &parent = reg.get_or_assign<Parent>(child.parent);

        parent.children.push_back(entity);
        if (!child.name.empty())
            parent.childrenByName[child.name] = entity;
    }

    void onChildDeletion(entt::registry &reg, entt::entity entity)
    {
        Child &child = reg.get<Child>(entity);
        Parent &parent = reg.get_or_assign<Parent>(child.parent);

        if (!child.name.empty())
            parent.childrenByName.erase(child.name);

        for (int i = 0; i < parent.children.size(); i++)
        {
            if (parent.children[i] != entity)
                continue;

            parent.children[i] = parent.children.back();
            parent.children.pop_back();
        }
    }

    void onParentDeletion(entt::registry &reg, entt::entity entity)
    {
        Parent &parent = reg.get_or_assign<Parent>(entity);
        if (parent.deleteChildrenOnDeletion)
            for (auto child : parent.children)
                reg.destroy(child);
    }

    void update(double deltaTime, Room *room) override
    {

    }

};


#endif
