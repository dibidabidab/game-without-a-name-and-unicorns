
#include "gu/game_utils.h"

#include "LevelScreen.h"

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

    LevelScreen scr;

    gu::setScreen(&scr);

    gu::run();

    return 0;
}
