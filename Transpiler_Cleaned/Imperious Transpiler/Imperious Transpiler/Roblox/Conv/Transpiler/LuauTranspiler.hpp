#include <iostream>
#include <string>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <functional>
#include <iterator>

extern "C" {
#include "../../../Libs/Lua/lua.h"
#include "../../../Libs/Lua\lualib.h"
#include "../../../Libs/Lua\lauxlib.h"
#include "../../../Libs/Lua\lobject.h"
#include "../../../Libs/Lua\lstring.h"
#include "../../../Libs/Lua\lfunc.h"
#include "../../../Libs/Lua\ldo.h"
#include "../../../Libs/Lua\lopcodes.h"
#include "../../../Libs/Lua\lstring.h"
}

namespace LuauTranspiler {
	std::string compile(lua_State* L, std::string source);
	Proto* convert(lua_State* L, Proto* p);
};