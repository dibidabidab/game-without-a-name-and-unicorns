
#include "gu/game_utils.h"
#include "test_screens/mesh_screen.cpp"

int main()
{
    gu::Config config;
    config.width = 1600;
    config.height = 900;
    config.title = "My game";
    config.showFPSInTitleBar = true; // note: this option will hide the default title.
    config.vsync = false;
    config.samples = 4;
    if (!gu::init(config))
        return -1;

    std::cout << "Running game with OpenGL version: " << glGetString(GL_VERSION) << "\n";

    // TODO: create your own screen that extends the Screen class (gu/screen.h)
    MeshScreen scr;

    gu::setScreen(&scr);

    gu::run();
    
    return 0;
}
