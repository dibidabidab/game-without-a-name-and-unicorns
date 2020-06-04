
#include "JsonEntityTemplate.h"
#include "../../../macro_magic/component.h"

JsonEntityTemplate::JsonEntityTemplate(const char *assetName) : jsonObj(assetName)
{

}

entt::entity JsonEntityTemplate::create()
{
    try
    {
        return fromJson(jsonObj->obj);
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception while creating entity from json template '" << jsonObj.getLoadedAsset().fullPath << "':\n" << e.what() << std::endl;
        return entt::null;
    }
}

entt::entity JsonEntityTemplate::fromJson(const json &j, entt::entity parent)
{
    entt::entity e;

    bool extends = j.contains("extends");

    if (extends)
        e = room->getTemplate(std::string(j["extends"]))->create();
    else
        e = room->entities.create();


    if (j.contains("children")) for (auto &childJson : j["children"].items())
    {
        auto child = fromJson(childJson.value(), e);
        setParent(child, e, childJson.key().c_str());
    }

    if (j.contains("components")) for (auto &el : j["components"].items())
    {
        auto utils = ComponentUtils::getFor(el.key());
        if (!utils)
            throw gu_err("Component '" + el.key() + "' does not exist!");

        json componentJson;
        if (extends && utils->entityHasComponent(e, &room->entities))
            utils->getJsonObjectComponent(componentJson, e, &room->entities);
        else
            componentJson = utils->constructObject();

        componentJson.merge_patch(el.value()); // todo: optimization: dont merge with default values, make a util function that takes an incomplete component

        // check for @ values in componentJson:
        std::function<void(json &)> check = [&] (json &j) {

            if (j.is_structured())
                for (auto &checkEl : j.items())
                    check(checkEl.value());
            else if (j.is_string() && std::string(j)[0] == '@')
            {
                auto parts = splitString(j, ":");
                if (parts.empty())
                    return;

                if (parts[0] == "@parent")
                    j = parent;
                else if (parts[0] == "@child" && parts.size() == 2)
                    j = room->getChildByName(e, parts[1].c_str());
            }

        };
        check(componentJson);

        utils->setJsonObjectComponent(componentJson, e, &room->entities);
    }

    return e;
}
