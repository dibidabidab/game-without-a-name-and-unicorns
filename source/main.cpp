
#include "gu/game_utils.h"
#include "test_screens/instanced_mesh_screen.cpp"

#include "../entt/src/entt/entity/registry.hpp"

struct position {
    float x = 0;
    float y = 0;
};

struct velocity {
    float dx = 0;
    float dy = 0;
};

void update(entt::registry &registry) {
    auto view = registry.view<position>();

    for(auto entity: view) {
        // gets only the components that are going to be used ...

        auto &pos = view.get<position>(entity);

        std::cout << "pos\n";

        // ...
    }
}

void update(std::uint64_t dt, entt::registry &registry) {
    registry.view<position, velocity>().each([dt](auto &pos, auto &vel) {
        // gets all the components of the view at once ...

        pos.x += vel.dx * dt;
        pos.y += vel.dy * dt;

        std::cout << "pos vel\n";
        std::cout << pos.x << ", " << pos.y << '\n';
        // ...
    });
}

void onVelAdded(entt::registry &reg, entt::entity entity)
{
    static int i = 0;

    auto &vel = reg.get<velocity>(entity);
    vel.dx = i++;
    vel.dy = i++;

    std::cout << "vel added\n";
}

int main() {
    entt::registry registry;
    std::uint64_t dt = 16;

    registry.on_construct<velocity>().connect<onVelAdded>();

    for(auto i = 0; i < 10; ++i) {
        auto entity = registry.create();
        registry.assign<position>(entity, i * 1.f, i * 1.f);
        if(i % 2 == 0) { registry.assign<velocity>(entity, i * .1f, i * .1f); }
    }

    update(dt, registry);
    update(registry);

//    gu::Config config;
//    config.width = 1900;
//    config.height = 900;
//    config.title = "My game";
//    config.showFPSInTitleBar = true; // note: this option will hide the default title.
//    config.vsync = true;
//    config.samples = 4;
//    if (!gu::init(config))
//        return -1;
//
//    std::cout << "Running game with OpenGL version: " << glGetString(GL_VERSION) << "\n";
//
//    // TODO: create your own screen that extends the Screen class (gu/screen.h)
//    InstancedMeshScreen scr;
//
//    gu::setScreen(&scr);
//
//    gu::run();

    return 0;
}
