
#ifndef GAME_JSONENTITYTEMPLATE_H
#define GAME_JSONENTITYTEMPLATE_H

#include "EntityTemplate.h"
#include "../../room/Room.h"

struct JsonEntityTemplateJson
{
    json obj;
};

class JsonEntityTemplate : public EntityTemplate
{

    const asset<JsonEntityTemplateJson> jsonObj;

  public:

    JsonEntityTemplate(const char *assetName);

    entt::entity create() override;

  private:

    entt::entity fromJson(const json &, entt::entity parent=entt::null);

};


#endif
