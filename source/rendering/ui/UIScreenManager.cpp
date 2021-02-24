
#include "UIScreenManager.h"

static UIScreen *deleteLater = NULL;

UIScreen &UIScreenManager::getActiveScreen()
{
    if (deleteLater && !deleteLater->isRenderingOrUpdating())
    {
        delete deleteLater;
        deleteLater = NULL;
    }
    if (noScreens())
        throw gu_err("There's no Active Screen!\n(If you're getting this error while opening a screen: a screen becomes active AFTER the script has run)");
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
    delete deleteLater;
    deleteLater = NULL;

    getActiveScreen().events.emit(0, "BeforeDelete");

    if (getActiveScreen().isRenderingOrUpdating())
        deleteLater = &getActiveScreen();
    else
        delete &getActiveScreen();

    screens.pop_back();
}

UIScreenManager::UIScreenManager()
{
    onResize = gu::onResize += [&] {
        for (auto &s : screens)
            s->onResize();
    };

    luau::getLuaState()["openScreen"] = [&] (const asset<luau::Script> &s) {
        this->openScreen(s);
    };
    luau::getLuaState()["closeActiveScreen"] = [&] {
        this->closeActiveScreen();
    };
}

UIScreenManager::~UIScreenManager()
{
    while (!noScreens())
        closeActiveScreen();
}
