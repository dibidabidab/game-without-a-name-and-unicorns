
#ifndef GAME_COMBATEFFECTSRENDERER_H
#define GAME_COMBATEFFECTSRENDERER_H

#include <graphics/camera.h>
#include <graphics/shader_asset.h>
#include "../../../../external/dibidab-engine/external/entt/src/entt/entity/registry.hpp"

class CombatEffectsRenderer
{

    ShaderAsset arrowReflectedShader;

  public:

    CombatEffectsRenderer();

    void render(const Camera &, entt::registry &, float deltaTime);

};


#endif
