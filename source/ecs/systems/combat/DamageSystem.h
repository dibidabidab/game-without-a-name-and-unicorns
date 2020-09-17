
#ifndef GAME_DAMAGESYSTEM_H
#define GAME_DAMAGESYSTEM_H

#include "../EntitySystem.h"
#include "../../../macro_magic/serializable.h"
#include "../../components/combat/Health.h"
#include <asset_manager/asset.h>

struct DamageType;

class DamageSystem : public EntitySystem
{

    asset<json> damageTypesJson;

    std::map<std::string, DamageType> damageTypes;

  protected:

    void loadDamageTypes();

    void update(double deltaTime, EntityEngine *room) override;

  public:
    DamageSystem();

};


#endif
