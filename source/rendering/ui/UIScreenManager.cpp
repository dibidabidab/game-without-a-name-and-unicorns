
#include "UIScreenManager.h"

UIScreen &UIScreenManager::getActiveScreen()
{
    assert(!noScreens());
    return *screens.back();
}

bool UIScreenManager::noScreens()
{
    return screens.empty();
}

void UIScreenManager::openScreen(const asset<luau::Script> &s)
{
    screens.push_back(new UIScreen(s));
}

void UIScreenManager::closeActiveScreen()
{
    delete &getActiveScreen();
    screens.pop_back();
}

UIScreenManager::UIScreenManager()
{
    onResize = gu::onResize += [&] {
        for (auto &s : screens)
            s->onResize();
    };
}
