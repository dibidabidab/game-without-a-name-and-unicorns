
#ifndef GAME_NETWORKINGSYSTEM_H
#define GAME_NETWORKINGSYSTEM_H


#include "LevelSystem.h"
#include "../../Level.h"
#include "../components/Networked.h"

class NetworkingSystem : public LevelSystem
{
  protected:
    void update(double deltaTime, Level *lvl) override
    {
        lvl->entities.view<Networked>().each([&](auto e, Networked &networked) {

            for (auto *c : networked.toSend)
            {
                json jsonToSend;
                bool hasChanged, isPresent;
                c->componentToJsonIfChanged(hasChanged, isPresent, jsonToSend, e, lvl->entities);

                bool wasPresent = networked.componentPresence[c->getComponentTypeHash()];

                if (wasPresent && !isPresent)
                {
                    std::cout << c->getComponentTypeName() << " was deleted from " << int(e) << "\n";
                }

                if (hasChanged)
                {
                    std::cout << c->getComponentTypeName() << " changed for " << int(e) << ":\n" << jsonToSend << "\n";
                }
            }

        });
    }
};


#endif
