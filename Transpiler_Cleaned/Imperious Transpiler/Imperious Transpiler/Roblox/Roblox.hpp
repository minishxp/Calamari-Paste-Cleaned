#pragma once
#include <cstdint>
#include <queue> 
#include "Updated.hpp"
#include "../Bypasses/Retcheck.hpp"

std::queue<std::string> ScriptQueue;

DWORD Add(DWORD func);

#define ASLR(x)(x - 0x400000 + (DWORD)GetModuleHandleA(0))
#define r_lua_pop(rL,n)		r_lua_settop(rL, -(n)-1)
#define r_lua_getglobal(rL,s)	r_lua_getfield(rL, LUA_GLOBALSINDEX, (s))
#define r_lua_setglobal(rL,s)	r_lua_setfield(rL, LUA_GLOBALSINDEX, (s))

std::uintptr_t RState;
lua_State* LS;

using getdatamodel_DEF = void(getdatamodel_CCV*)(std::uintptr_t, std::uintptr_t);
using getdatamodel2_DEF = DWORD(getdatamodel2_CCV*)();
using deserialize_DEF = int(luau_deserialize_CCV*)(std::uintptr_t, const char*, const char*, size_t, int);
using spawn_DEF = void(spawn_CCV*)(std::uintptr_t);
using getfield_DEF = int(lua_getfield_CCV*)(std::uintptr_t, int, const char* );
using setfield_DEF = void(lua_setfield_CCV*)(std::uintptr_t, int, const char*);
using pushvalue_DEF = void(lua_pushvalue_CCV*)(std::uintptr_t, std::uintptr_t);
using pushcclosure_DEF = void(lua_pushcclosure_CCV*)(std::uintptr_t, int, int, int, int);
using newthread_DEF = DWORD(lua_newthread_CCV*)(std::uintptr_t);
using settop_DEF = void(lua_settop_CCV*)(std::uintptr_t, int);
using pcall_DEF = int(lua_pcall_CCV*)(std::uintptr_t, int, int, int);
using pushstring_DEF = void(lua_pushstring_CCV*)(std::uintptr_t , const char*);
using call_DEF = void(lua_call_CCV*)(std::uintptr_t, std::uintptr_t, std::uintptr_t);

deserialize_DEF r_lua_deserialize = (deserialize_DEF)unprotect(ASLR(luau_deserialize_addr));
spawn_DEF r_lua_spawn = (spawn_DEF)unprotect(ASLR(spawn_addr));
getfield_DEF r_lua_getfield = (getfield_DEF)(unprotect(ASLR(lua_getfield_addr)));
setfield_DEF r_lua_setfield = (setfield_DEF)unprotect(ASLR(lua_setfield_addr));
pushvalue_DEF r_lua_pushvalue = (pushvalue_DEF)(unprotect(ASLR(lua_pushvalue_addr)));
pushcclosure_DEF r_luax_pushcclosure = (pushcclosure_DEF)(unprotect(ASLR(lua_pushcclosure_addr)));
newthread_DEF r_lua_newthread = (newthread_DEF)(unprotect(ASLR(lua_newthread_addr)));
settop_DEF r_lua_settop = (settop_DEF)(unprotect(ASLR(lua_settop_addr)));
pcall_DEF r_lua_pcall = (pcall_DEF)(unprotect(ASLR(lua_pcall_addr)));
pushstring_DEF r_lua_pushstring = (pushstring_DEF)(unprotect(ASLR(lua_pushstring_addr)));
call_DEF r_lua_call = (call_DEF)(unprotect(ASLR(lua_call_addr)));


void r_lua_pushcclosure(std::uintptr_t RS, int Func, int upvals)
{
	DWORD Func2 = Add(Func);
	r_luax_pushcclosure(RS, Func2, NULL, upvals, NULL);
}

void r_lua_pushcfunction(std::uintptr_t RS, int Func)
{
	r_lua_pushcclosure(RS, Func, 0);
}

void RegisterFunction(const char* Name, int Function(DWORD))
{
	r_lua_pushcfunction(RState, (DWORD)Function);
	r_lua_setglobal(RState, Name);
}

std::uintptr_t GetDataModel()
{
	DWORD DMPad[8];
	DWORD GetDataModelAdr = ASLR(getdatamodel_addr);
	DWORD GetDataModel2Adr = ASLR(getdatamodel2_addr);
	reinterpret_cast<getdatamodel_DEF>(GetDataModelAdr)(reinterpret_cast<getdatamodel2_DEF>(GetDataModel2Adr)(),
		(DWORD)DMPad);
	return DMPad[0] + 0xC;
}

const char* GetClass(int self)
{
	return (const char*)(*(int(**)(void))(*(int*)self + 16))();
}

int FindChildOfInstance(int Instance, const char* Name)
{
	DWORD StartOfChildren = *(DWORD*)(Instance + 0x2C);
	DWORD EndOfChildren = *(DWORD*)(StartOfChildren + 4);

	for (int i = *(int*)StartOfChildren; i != EndOfChildren; i += 8)
	{
		if (memcmp(GetClass(*(int*)i), Name, strlen(Name)) == 0)
		{
			return *(int*)i;
		}
	}
}