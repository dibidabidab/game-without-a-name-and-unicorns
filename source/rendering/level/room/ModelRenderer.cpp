
#include <graphics/3d/vert_buffer.h>
#include <graphics/3d/perspective_camera.h>

#include "ModelRenderer.h"
#include "../../../generated/ModelView.hpp"
#include "../../../game/Game.h"

void ModelRenderer::render(entt::registry &registry, const OrthographicCamera &camera)
{
    PerspectiveCamera perspectiveCam(.1, 1000, camera.viewportWidth, camera.viewportHeight, 70);
    perspectiveCam.position = mu::Z;
    perspectiveCam.lookAt(mu::ZERO_3);
    perspectiveCam.position = camera.position;
    perspectiveCam.update();

    shader.use();

    uint8 prevColor = 0;

    registry.view<ModelView>().each([&] (auto e, ModelView &view) {

        if (view.prevModelName != view.modelName || view.modelCollection.hasReloaded())
        {
            view.model = NULL;

            for (auto model : view.modelCollection.get())
                if (model->name == view.modelName)
                {
                    view.model = model;
                    break;
                }

            view.prevModelName = view.modelName;
        }
        if (view.model == NULL)
            return;

        mat4 mvp(1.0f);

        mvp = glm::translate(mvp, view.locationOffset);

        // rotation.. http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-17-quaternions/
        mvp *= glm::toMat4(view.rotation);

        mvp = glm::scale(mvp, view.scale);

        mvp = (view.renderWithPerspectiveCamera ? perspectiveCam.combined : camera.combined) * mvp;

        glUniformMatrix4fv(shader.location("mvp"), 1, GL_FALSE, &mvp[0][0]);

        for (auto &part : view.model->parts)
        {
            if (!part.mesh->vertBuffer->isUploaded())
                part.mesh->vertBuffer->upload(true);

            uint8 color = 0u;

            if (part.material)
            {
                auto c = colorNameMap.find(part.material->name);
                if (c != colorNameMap.end())
                    color = c->second;
            }
            if (color != prevColor)
                glUniform1ui(shader.location("colorIndex"), color);

            part.mesh->render(part.meshPartIndex);
        }
    });
}

ModelRenderer::ModelRenderer()
    : shader("3d model shader", "shaders/3d_model.vert", "shaders/3d_model.frag")
{
    Palette &palette = Game::palettes->effects.at(0).lightLevels[0].get();
    uint8 i = 0;
    for (auto &[name, color] : palette.colors)
        colorNameMap[name] = i++;

}
