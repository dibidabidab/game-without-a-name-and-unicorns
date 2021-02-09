
#ifndef GAME_TILEDROOMENTITYINSPECTOR_H
#define GAME_TILEDROOMENTITYINSPECTOR_H


#include <ecs/EntityInspector.h>

class TiledRoomEntityInspector : public EntityInspector
{
    using EntityInspector::EntityInspector;

    void pickEntityGUI(const Camera *, DebugLineRenderer &) override;

    void moveEntityGUI(const Camera *, DebugLineRenderer &) override;

  protected:
    void highLightEntity(entt::entity entity, const Camera *camera, DebugLineRenderer &renderer) override;
};


#endif
