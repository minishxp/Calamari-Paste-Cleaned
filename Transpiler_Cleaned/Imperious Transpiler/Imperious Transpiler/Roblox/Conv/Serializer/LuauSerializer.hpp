#include <iostream>
#include <string>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <functional>
#include <iterator>

extern "C" {
#include "../../../Libs/Lua\lua.h"
#include "../../../Libs/Lua\lualib.h"
#include "../../../Libs/Lua\lauxlib.h"
#include "../../../Libs/Lua\lobject.h"
#include "../../../Libs/Lua\lstring.h"
#include "../../../Libs/Lua\lfunc.h"
#include "../../../Libs/Lua\ldo.h"
#include "../../../Libs/Lua\lopcodes.h"
#include "../../../Libs/Lua\lstring.h"
}

static const char kBytecodeMagic[] = "RSB1";
static const unsigned int kBytecodeHashSeed = 42;
static const unsigned int kBytecodeHashMultiplier = 41;

enum BytecodeConstantType
{
	Constant_Nil,
	Constant_Bool,
	Constant_Number,
	Constant_String,
	Constant_Method,
	Constant_Global,
};

class LuauSerializer {
public:
	static std::string writeClosure(lua_State* L);
	static int readClosure(lua_State* L, const char* chunkname, const char* source, size_t len);
};