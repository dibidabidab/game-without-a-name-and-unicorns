
#include "gu/game_utils.h"

#include "LevelScreen.h"
#include "level/ecs/networking/NetworkedComponent.h"
#include "macro_magic/json_reflectable.h"
#include "macro_magic/component.h"

int main() {
    gu::Config config;
    config.width = 1900;
    config.height = 900;
    config.title = "My game";
    config.showFPSInTitleBar = true; // note: this option will hide the default title.
    config.vsync = false;
    config.samples = 0;
    if (!gu::init(config))
        return -1;

    std::cout << "Running game with OpenGL version: " << glGetString(GL_VERSION) << "\n";

//    LevelScreen scr;
//
//    gu::setScreen(&scr);
//
//    gu::run();

    NetworkedComponent<Test> h;
//    NetworkedComponent<Test2> h2;
//    NetworkedComponent<Test2> h3;

    std::cout << h.getComponentTypeHash() << '\n';
//    std::cout << h2.getComponentTypeHash() << '\n';
//    std::cout << h3.getComponentTypeHash() << '\n';

    return 0;
}
