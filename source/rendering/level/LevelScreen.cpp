
#include "LevelScreen.h"
#include "MiniMapTextureGenerator.h"
#include "../../level/room/ecs/components/PlayerControlled.h"
#include "../../level/room/ecs/components/TransRoomable.h"

LevelScreen::LevelScreen(Level *lvl, int localPlayerID) : lvl(lvl), localPlayerID(localPlayerID), lvlEditor(lvl)
{
    lvl->onPlayerLeftRoom = [this](Room *room, int playerId)
    {
        if (this->localPlayerID != playerId || (roomScreen && roomScreen->room != room))
            return;
        delete roomScreen;
        roomScreen = NULL;
    };

    lvl->onPlayerEnteredRoom = [this](Room *room, int playerId)
    {
        if (this->localPlayerID != playerId)
            return;
        std::cout << "Local player entered room. Show RoomScreen\n";
        showRoom(room);
    };

    lvl->beforeRoomDeletion = [this](Room *r)
    {
        if (roomScreen && r == roomScreen->room)
        {
            delete roomScreen;
            roomScreen = NULL;
        }
    };
}

void LevelScreen::render(double deltaTime)
{
    glClearColor(32 / 255., 53 / 255., 189 / 255., 1);
    glClear(GL_COLOR_BUFFER_BIT);

    renderDebugTools();
    if (roomScreen)
        roomScreen->render(deltaTime);
}

void LevelScreen::onResize()
{
    if (roomScreen)
        roomScreen->onResize();
}

void LevelScreen::renderDebugTools()
{
    if (!Game::settings.showDeveloperOptions)
        return;

    ImGui::BeginMainMenuBar();

    if (ImGui::BeginMenu("Level"))
    {
        auto str = std::to_string(lvl->getNrOfRooms()) + " room(s) active.";
        ImGui::MenuItem(str.c_str(), NULL, false, false);

        if (ImGui::MenuItem("Level editor"))
            lvlEditor.show = true;

        ImGui::EndMenu();
    }

    ImGui::EndMainMenuBar();

    lvlEditor.render();
    if (lvlEditor.moveCameraToRoom >= 0)
        showRoom(&lvl->getRoom(lvlEditor.moveCameraToRoom));
}

void LevelScreen::showRoom(Room *r)
{
    auto old = roomScreen;
    roomScreen = NULL;
    if (r)
    {
        roomScreen = new RoomScreen(r, true);
        roomScreen->onResize();
        if (old)
            roomScreen->currentPaletteEffect = roomScreen->prevPaletteEffect = old->currentPaletteEffect;

        if (Game::settings.graphics.roomTransitionAnimation)
            r->entities.view<LocalPlayer, TransRoomed>().each([&](auto, TransRoomed &t) {
                roomScreen->camMovement.offsetAnim = normalize(vec2(t.travelDir)) * -10.f;
            });
    }
    delete old;
}

