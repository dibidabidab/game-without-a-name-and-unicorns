
#ifndef GAME_LUAU_H
#define GAME_LUAU_H

/**
 * lua/sol3 utils
 */

extern "C" {
    #include "lua.h"
    #include "lauxlib.h"
    #include "lualib.h"
}
#define SOL_ALL_SAFETIES_ON 1
#include "../external/lua/sol2/single/include/sol/sol.hpp"

namespace luau
{

}

#endif //GAME_LUAU_H
