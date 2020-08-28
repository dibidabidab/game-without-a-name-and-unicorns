
#ifndef GAME_TRANSROOMABLE_H
#define GAME_TRANSROOMABLE_H

#include "../../../../macro_magic/component.h"

using children_comps_map = std::map<std::string, std::vector<std::string>>;

COMPONENT(TransRoomable, HASH(0),

    FIELD(std::string, templateName),
    FIELD(std::vector<std::string>, archiveComponents),
    FIELD(children_comps_map, archiveChildComponents)
)
END_COMPONENT(TransRoomable)

COMPONENT(TransRoomed, HASH(0),

    FIELD(std::vector<int>, travelHistory),
    FIELD_DEF_VAL(json, archivedComponents, json::object()),
    FIELD_DEF_VAL(json, archivedChildComponents, json::object()),
    FIELD(ivec2, positionInNewRoom),
    FIELD(ivec2, travelDir)
)
END_COMPONENT(TransRoomed)

#endif //GAME_TRANSROOMABLE_H
