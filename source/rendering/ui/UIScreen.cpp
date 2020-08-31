
#include "UIScreen.h"

UIScreen::UIScreen(const asset<luau::Script> &s) : script(s)
{
    initialize();

    luau::getLuaState().script(script->getByteCode().as_string_view(), luaEnvironment);
}

void UIScreen::initializeLuaEnvironment()
{
    EntityEngine::initializeLuaEnvironment();


}

void UIScreen::render(double deltaTime)
{
}

void UIScreen::onResize()
{
}
