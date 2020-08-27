
#ifndef GAME_TRANSROOMABLE_H
#define GAME_TRANSROOMABLE_H

#include "../../../macro_magic/component.h"

COMPONENT(TransRoomable, HASH(0),

    FIELD(std::string, templateName),
    FIELD(std::vector<std::string>, archiveComponents)
)
END_COMPONENT(TransRoomable)

COMPONENT(TransRoomed, HASH(0),

    FIELD(std::vector<int>, travelHistory),
    FIELD_DEF_VAL(json, archivedComponents, json::object()),
    FIELD(ivec2, positionInNewRoom)
)
END_COMPONENT(TransRoomed)

#endif //GAME_TRANSROOMABLE_H
