
#include <Windows.h>
#include <thread>
#include <string>
#include <iostream>
#include <queue>
#include "Roblox/Conv/Transpiler/LuauTranspiler.hpp"
#include "Roblox/Roblox.hpp"
#include "Bypasses/CallCheck.hpp"


bool DebugConsole(const char* Name)
{
	VirtualProtect((PVOID)&FreeConsole, 1, PAGE_EXECUTE_READWRITE, new unsigned long);
	*(BYTE*)(&FreeConsole) = 0xC3;
	AllocConsole();
	SetConsoleTitleA(Name);
	freopen("CONOUT$", "w", stdout);
	freopen("CONIN$", "r", stdin);
	HWND ConsoleHandle = GetConsoleWindow();
	::SetWindowPos(ConsoleHandle, HWND_TOPMOST, 0, 0, 0, 0, SWP_DRAWFRAME | SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
	::ShowWindow(ConsoleHandle, SW_NORMAL);
	return true;
}

std::uintptr_t ExecuteScript(int RS, std::string Script) 
{
	LS = luaL_newstate();
	luaL_openlibs(LS);
	auto Bytecode = LuauTranspiler::compile(lua_open(), Script);

	if (luaL_loadbuffer(LS, Script.c_str(), Script.size(), "@Transpiler"))
	{
		const std::string error = lua_tostring(LS, -1);
		lua_close(LS);
		throw std::exception(error.c_str());
	}
	else
	{
		r_lua_deserialize(RS, "Transpiler", Bytecode.c_str(), Bytecode.size(), 0);
		r_lua_spawn(RS);
	}
	return true;
}

int RenderStepHook(int RS)
{
	if (!ScriptQueue.empty()) 
	{
		auto script = ScriptQueue.front();
		ExecuteScript(RS, script);
		ScriptQueue.pop();
	}
	return 1;
}

int LuaPipe()
{
	HANDLE hPipe;
	char buffer[999999];
	DWORD dwRead;


	hPipe = CreateNamedPipe(TEXT("\\\\.\\pipe\\Fluxus_Is_Axon"),
		PIPE_ACCESS_DUPLEX,
		PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
		1,
		9999999,
		9999999,
		NMPWAIT_USE_DEFAULT_WAIT,
		NULL);

	while (hPipe != INVALID_HANDLE_VALUE)
	{
		if (ConnectNamedPipe(hPipe, NULL) != FALSE)  
		{
			while (ReadFile(hPipe, buffer, sizeof(buffer) - 1, &dwRead, NULL) != FALSE)
			{
				buffer[dwRead] = '\0';

				if (strlen(buffer) != NULL) {
					ScriptQueue.push(buffer);
				}
			}
		}

		DisconnectNamedPipe(hPipe);
	}
}

unsigned long Entry(void)
{
	DebugConsole("Calamari Cleaned | Vortex#6666");

	{
		std::uintptr_t DataModel = GetDataModel();
		std::uintptr_t ScriptContext = FindChildOfInstance(DataModel, "ScriptContext");
		RState = RBX_LuaState(ScriptContext);
		RBX_SetIdentity(RState, 8);
		BypassCallCheck();
	}

	{
		r_lua_getfield(RState, LUA_GLOBALSINDEX, "game");
		r_lua_getfield(RState, -1, "GetService");
		r_lua_pushvalue(RState, -2);
		r_lua_pushstring(RState, "RunService");
		r_lua_pcall(RState, 2, 1, 0);
		r_lua_getfield(RState, -1, "RenderStepped");
		r_lua_getfield(RState, -1, "Connect");
		r_lua_pushvalue(RState, -2);
		r_lua_pushcclosure(RState, (DWORD)RenderStepHook, 0);
		r_lua_pcall(RState, 2, 0, 0);
		r_lua_pop(RState, 2);
	}

	std::thread(LuaPipe).detach();

	while (true) 
	{
		std::string Data = "";
		std::getline(std::cin, Data);
		ExecuteScript(r_lua_newthread(RState), Data);
	}

	return 1;
}

int _stdcall DllMain(HMODULE mod, std::uintptr_t reason, void*) 
{
	if (reason == 1)
		std::thread(Entry).detach();
	return 1;
}
