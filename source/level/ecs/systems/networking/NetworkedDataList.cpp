
#include "NetworkedDataList.h"
#include "NetworkedDataGroup.h"

NetworkedDataGroup &NetworkedDataList::group()
{
    auto group = new NetworkedDataGroup(this);
    return *group;
}
