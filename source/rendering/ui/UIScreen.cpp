
#include "UIScreen.h"
#include "../../game/Game.h"
#include "../../game/session/SingleplayerSession.h"

UIScreen::UIScreen(const asset<luau::Script> &s) : script(s)
{
    initialize();
    UIScreen::onResize();

    try
    {
        luau::getLuaState().unsafe_script(script->getByteCode().as_string_view(), luaEnvironment);
    }
    catch (std::exception &e)
    {
        std::cerr << "Error while running UIScreen script " << script.getLoadedAsset().fullPath << ":" << std::endl;
        std::cerr << e.what() << std::endl;
    }
}

void UIScreen::initializeLuaEnvironment()
{
    EntityEngine::initializeLuaEnvironment();

    luaEnvironment["endCurrentSession"] = [] {
        Game::setCurrentSession(NULL);
    };

    luaEnvironment["startSinglePlayerSession"] = [] (const char *saveGamePath) {
        Game::setCurrentSession(new SingleplayerSession(saveGamePath));
    };
    // todo: startMultiplayerServerSession and startMultiplayerClientsession

    luaEnvironment["joinSession"] = [] (const char *username, const sol::function& onJoinRequestDeclined) {

        auto &session = Game::getCurrentSession();

        session.onJoinRequestDeclined = [onJoinRequestDeclined] (auto reason) {

            sol::protected_function_result result = onJoinRequestDeclined(reason);
            if (!result.valid())
                throw gu_err(result.get<sol::error>().what());
        };
        session.join(username);
    };

    luaEnvironment["loadOrCreateLevel"] = [](const char *path)
    {
        auto &session = Game::getCurrentSession();
        auto singleplayerSession = dynamic_cast<SingleplayerSession *>(&session);
        if (singleplayerSession)
            singleplayerSession->setLevel(new Level(path));
    };
}

void UIScreen::render(double deltaTime)
{
}

void UIScreen::onResize()
{
}
