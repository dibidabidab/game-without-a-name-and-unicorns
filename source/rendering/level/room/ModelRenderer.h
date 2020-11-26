
#ifndef GAME_MODELRENDERER_H
#define GAME_MODELRENDERER_H


#include <graphics/orthographic_camera.h>
#include <graphics/shader_asset.h>
#include "../../../../external/dibidab-engine/external/entt/src/entt/entity/registry.hpp"

class ModelRenderer
{

    ShaderAsset shader;
    std::map<std::string, uint8> colorNameMap;

  public:

    ModelRenderer();

    void render(entt::registry &registry, const OrthographicCamera &camera);
};


#endif
