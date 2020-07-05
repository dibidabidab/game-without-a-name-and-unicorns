
#include "SpawningSystem.h"

void SpawningSystem::update(double deltaTime, Room *room)
{
    room->entities.view<DespawnAfter>().each([&](auto e, DespawnAfter &despawnAfter) {
        despawnAfter.timer += deltaTime;
        if (despawnAfter.timer >= despawnAfter.time)
            room->entities.destroy(e);
    });

    int spawnedE = 0;

    room->entities.view<TemplateSpawner>().each([&](auto e, TemplateSpawner &spawner) {

        try
        {
            auto spawned = room->entities.create();
            room->entities.assign<SpawnedBy>(spawned, e);

            room->getTemplate(spawner.templateName).createComponents(spawned);
            spawnedE++;
        }
        catch (_gu_err &err)
        {
            std::cerr << "TemplateSpawner#" << int(e) << " caused error:\n" << err.what() << std::endl;
        }
    });
//    std::cout << spawnedE << '\n';
}
