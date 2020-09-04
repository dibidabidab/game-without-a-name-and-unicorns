
#ifndef GAME_UISCREEN_H
#define GAME_UISCREEN_H


#include <graphics/orthographic_camera.h>
#include "../../ecs/EntityEngine.h"
#include "../../ecs/EntityInspector.h"
#include "TextRenderer.h"
#include "../../ecs/components/Children.h"

class UIScreen : public EntityEngine, public Screen
{
    asset<luau::Script> script;

    EntityInspector inspector;

    DebugLineRenderer lineRenderer;

    OrthographicCamera cam;

    TextRenderer textRenderer;

  protected:
    void initializeLuaEnvironment() override;

  public:

    UIScreen(const asset<luau::Script> &);

    void render(double deltaTime) override;

    void onResize() override;

    void renderDebugStuff();

  private:
    void renderFamily(const Parent &, double deltaTime);

    void renderChild(entt::entity childEntity, double deltaTime);
};


#endif
