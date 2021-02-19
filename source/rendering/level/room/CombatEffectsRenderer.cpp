
#include "CombatEffectsRenderer.h"
#include "../../../generated/CombatEffects.hpp"
#include "../../../generated/Physics.hpp"
#include <graphics/3d/mesh.h>

CombatEffectsRenderer::CombatEffectsRenderer()
    : arrowReflectedShader("arrow reflected shader", "shaders/combat/combat_effect.vert", "shaders/combat/arrow_reflected.frag"),
      damageFlashShader("damage flash shader", "shaders/combat/combat_effect.vert", "shaders/combat/damage_flash.frag")
{

}

void CombatEffectsRenderer::render(const Camera &cam, entt::registry &reg, float deltaTime)
{
    arrowReflectedShader.use();
    glUniformMatrix4fv(arrowReflectedShader.location("projection"), 1, GL_FALSE, &cam.combined[0][0]);

    reg.view<ArrowReflectedEffect, AABB>().each([&](auto e, auto &effect, AABB &aabb) {

        effect.timer += deltaTime * 1.5;

        if (effect.timer > 1.)
        {
            reg.remove<ArrowReflectedEffect>(e);
            return;
        }

        glUniform1f(arrowReflectedShader.location("time"), effect.timer);
        glUniform1f(arrowReflectedShader.location("attackAngle"), effect.angle);
        glUniform1ui(arrowReflectedShader.location("radius"), effect.radius);
        glUniform2i(arrowReflectedShader.location("position"), aabb.center.x, aabb.center.y);

        Mesh::getQuad()->render();
    });

    damageFlashShader.use();
    glUniformMatrix4fv(damageFlashShader.location("projection"), 1, GL_FALSE, &cam.combined[0][0]);

    reg.view<DamageFlashEffect, AABB>().each([&](auto e, auto &effect, AABB &aabb) {

        effect.timer += deltaTime * 5.;

        if (effect.timer > 1.)
        {
            reg.remove<DamageFlashEffect>(e);
            return;
        }

        glUniform1f(damageFlashShader.location("time"), effect.timer);
        glUniform1ui(damageFlashShader.location("radius"), effect.radius);
        glUniform2i(damageFlashShader.location("position"), aabb.center.x, aabb.center.y);

        Mesh::getQuad()->render();
    });
}
