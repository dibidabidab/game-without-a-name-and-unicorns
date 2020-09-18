
#ifndef GAME_NETWORKEDDATAGROUP_H
#define GAME_NETWORKEDDATAGROUP_H

#include "NetworkedData.h"

/**
 * Useful for sending MULTIPLE Components whenever ANY of them has changed.
 *
 * @tparam ComponentTypes structs made with the COMPONENT() macro
 */
struct NetworkedDataGroup : public AbstractNetworkedData, public NetworkedDataList
{
    std::string combinedName;
    hash combinedHash = 0;

  protected:
    void add(AbstractNetworkedData *d) override
    {
        NetworkedDataList::add(d);
        if (!combinedName.empty())
            combinedName += "&";

        combinedName += d->getDataTypeName();
        combinedHash += d->getDataTypeHash();
    }

  public:

    NetworkedDataList *parent;

    NetworkedDataGroup(NetworkedDataList *parent) : parent(parent) {}

    void endGroup() override
    {
        assert(list.size() > 1);
        parent->add(this);
    }

    hash getDataTypeHash() const override
    {
        return combinedHash;
    }

    const char *getDataTypeName() const override
    {
        return combinedName.c_str();
    }

    void update(double deltaTime, const entt::entity &e, entt::registry &reg) override
    {
        for (auto &d : list) d->update(deltaTime, e, reg);
    }

    void updateDataFromNetwork(const json &in, const entt::entity &e, entt::registry &reg) override
    {
        int i = 0;
        for (auto &d : list)
            d->updateDataFromNetwork(in.at(i++), e, reg);
    }

    void removeData(entt::entity e, entt::registry &reg) override
    {
        for (auto &d : list)
            d->removeData(e, reg);
    }

    void dataToJsonIfChanged(bool &hasChanged, bool &dataPresent, json &out, const entt::entity &e,
                             entt::registry &reg) override
    {
        out = json::array();
        for (auto &d : list)
        {
            out.push_back(json());
            d->dataToJsonIfChanged(hasChanged, dataPresent, out.back(), e, reg);
        }
        if (hasChanged && dataPresent)
        {
            int i = -1;
            for (auto &d : list)
                if (out.at(++i).is_null())
                    d->dataToJson(out.at(i), e, reg);
        }
    }

    void dataToJson(json &out, const entt::entity &e, entt::registry &reg) override
    {
        for (auto &d : list)
        {
            out.push_back(json());
            d->dataToJson(out.back(), e, reg);
        }
    }

};


#endif
