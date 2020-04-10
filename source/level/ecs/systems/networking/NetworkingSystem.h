
#ifndef GAME_NETWORKINGSYSTEM_H
#define GAME_NETWORKINGSYSTEM_H


#include "../EntitySystem.h"
#include "../../../Level.h"
#include "../../components/Networked.h"

class NetworkingSystem : public EntitySystem
{
    using EntitySystem::EntitySystem;
  protected:
    void init(Room *room) override
    {
        room->entities.on_construct<Networked>().connect<&NetworkingSystem::onCreated>(this);
        room->entities.on_destroy<Networked>().connect<&NetworkingSystem::onDestroyed>(this);
    }

    void onCreated(entt::registry &reg, entt::entity entity)
    {
        Networked &networked = reg.get<Networked>(entity);
        std::cout << "Entity " << int(entity) << ":" << networked.networkID << " created\n";
    }

    void onDestroyed(entt::registry &reg, entt::entity entity)
    {
        Networked &networked = reg.get<Networked>(entity);
        std::cout << "Entity " << int(entity) << ":" << networked.networkID << " destroyed\n";
    }

    void update(double deltaTime, Room *room) override
    {
        room->entities.view<Networked>().each([&](auto e, Networked &networked) {

            if (!networked.toSend) return;

            for (auto *c : networked.toSend->list)
            {
                gu::profiler::Zone z(c->getDataTypeName());
                json jsonToSend;
                bool hasChanged = false, isPresent = true;
                c->dataToJsonIfChanged(hasChanged, isPresent, jsonToSend, e, room->entities);

                bool wasPresent = networked.dataPresence[c->getDataTypeHash()];

                if (wasPresent && !isPresent)
                {
                    networked.dataPresence[c->getDataTypeHash()] = false;
                    std::cout << c->getDataTypeName() << " was deleted from " << int(e) << ":" << networked.networkID <<"\n";
                }

                if (hasChanged)
                {
//                    std::cout << c->getDataTypeName() << " changed for " << int(e) << ":" << networked.networkID << ":\n" << jsonToSend << "\n";
                }
            }
        });
    }
};


#endif
