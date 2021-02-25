
#ifndef GAME_UISCREEN_H
#define GAME_UISCREEN_H


#include <graphics/orthographic_camera.h>
#include <graphics/frame_buffer.h>
#include <ecs/EntityEngine.h>
#include <graphics/3d/debug_line_renderer.h>
#include <ecs/EntityInspector.h>
#include "TextRenderer.h"
#include "../sprites/SpriteRenderer.h"
#include "NineSliceRenderer.h"

class UIScreen : public EntityEngine, public Screen
{
    asset<luau::Script> script;

    EntityInspector inspector;

    OrthographicCamera cam;

    DebugLineRenderer lineRenderer;
    TextRenderer textRenderer;
    SpriteRenderer spriteRenderer;
    NineSliceRenderer nineSliceRenderer;

    FrameBuffer *indexedFbo = NULL;
    ShaderAsset applyPaletteUIShader;

    bool initialized = false;

    bool renderingOrUpdating = false;

    asset<Texture> transitionTexture;
    float transitionTimer = 0.;
    int transitionDir = 0; // 0 = none, 1 = to black, -1 = to transparent
    ShaderAsset *transitionShader = NULL;

  public:

    UIScreen(const asset<luau::Script> &);

    void render(double deltaTime) override;

    void onResize() override;

    void renderDebugStuff();

    bool isRenderingOrUpdating() const { return renderingOrUpdating; }

    ~UIScreen() override;

  private:
    void renderUIContainer(entt::entity, UIElement &, UIContainer &, UIContainer &parent, double deltaTime);

    void renderUIElement(entt::entity, UIElement &, UIContainer &, double deltaTime);
};


#endif
