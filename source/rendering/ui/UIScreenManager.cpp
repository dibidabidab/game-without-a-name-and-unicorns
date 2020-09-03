
#include "UIScreenManager.h"

UIScreen &UIScreenManager::getActiveScreen()
{
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

static UIScreen *deleteLater = NULL;

void UIScreenManager::closeActiveScreen()
{
    delete deleteLater;
    deleteLater = NULL;

    if (getActiveScreen().isUpdating())
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
}

UIScreenManager::~UIScreenManager()
{
    while (!noScreens())
        closeActiveScreen();
}
