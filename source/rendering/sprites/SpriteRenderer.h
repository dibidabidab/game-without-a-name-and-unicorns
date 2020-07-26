
#ifndef GAME_SPRITERENDERER_H
#define GAME_SPRITERENDERER_H


#include <graphics/3d/mesh.h>
#include <graphics/camera.h>
#include <graphics/shader_asset.h>
#include "MegaSpriteSheet.h"
#include "../../../external/entt/src/entt/entity/registry.hpp"
#include "../../level/ecs/components/graphics/AsepriteView.h"

/**
 * Used to render all entities with a AsepriteView component.
 */
class SpriteRenderer
{

    const MegaSpriteSheet *megaSpriteSheet;

    SharedMesh quad;
    VertData instancedData;
    int instancedDataID = -1;

    ShaderAsset shader;

  public:
    SpriteRenderer(const MegaSpriteSheet *);

    void render(double deltaTime, const Camera &cam, entt::registry &reg);

};


#endif
