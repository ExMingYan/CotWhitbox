#include "LuaExec.hpp"
#include "LuaExec.h"
#include "LuaAsm.h"
#include "LuaKey.h"
#include "LuaMemory.h"

#include "utils/Logger.hpp"
#include "utils/searcher.h"
#include "utils/ProcessInfo.h"

#include <chrono>
#include <string>
#pragma comment(lib, "Lua.lib")

void ExecuteLuaFile()
{
	fs::path LuaPath = GetLuaScriptFolder();
	LuaOnceWithoutThread(LuaPath);
	std::thread OnceThread(LuaOnceWithThread, LuaPath);
	OnceThread.detach();
	std::thread AlwaysThread(LuaAlwaysWithThread, LuaPath);
	AlwaysThread.detach();
}

fs::path GetLuaScriptFolder()
{
	fs::path processPath = fs::path(GetModuleFolderPath());
	fs::path ScriptFolder = processPath / "LuaProxy";
	return ScriptFolder;
}

LuaExec::LuaExec()
{
	L = luaL_newstate();
	luaL_openlibs(L);
	lua_register(L, "CheckGameVersion", Lua_CheckGameVersion);
	lua_register(L, "LoadDLL", Lua_LoadLibrary);
	lua_register(L, "LoadDLLWithEntry", Lua_LoadLibraryEntry);
	lua_register(L, "Sleep", Lua_Sleep);
	InitMemoryAccess(L);
	InitKeyFunctions(L);
	InitLuaAsmFunction(L);
}

LuaExec::~LuaExec()
{
	if (L)
	{
		lua_close(L);
		L = nullptr;
	}
}

bool LuaExec::ExecuteFile(const char* filename)
{
	if (luaL_dofile(L, filename) != LUA_OK)
	{
		const char* errorMsg = lua_tostring(L, -1);
		loggers::information(__FUNCTION__, __LINE__, "Lua Error: %s", errorMsg);
		lua_pop(L, 1);
		return false;
	}
	int32_t nRet = lua_gettop(L);
	if (nRet > 0)
	{
		return ProcessReturnValues(nRet);
	}
	return true;
}

static int Lua_CheckGameVersion(lua_State* L)
{
	const char* version = luaL_checkstring(L, 1);
	const std::wstring versionW = std::wstring(version, version + strlen(version));
	bool match = CheckGameVersion(versionW);
	lua_pushboolean(L, match);
	return 1;
}

static int Lua_LoadLibrary(lua_State* L)
{
	int n = lua_gettop(L);
	const char* dllPath = luaL_checkstring(L, 1);
	bool waitForWindow = false;
	if (n >= 2)
	{
		waitForWindow = lua_toboolean(L, 2);
	}

	if (waitForWindow)
	{
		const char* windowClass = luaL_checkstring(L, 3);
		WaitForMainWindow(std::string(windowClass));
	}

	HMODULE hModule = LoadLibraryA(dllPath);
	if (hModule)
	{
		lua_pushboolean(L, true);
	}
	else
	{
		lua_pushboolean(L, false);
	}
	return 1;
}

static int Lua_LoadLibraryEntry(lua_State* L)
{
	const char* dllPath = luaL_checkstring(L, 1);
	const char* entryName = luaL_checkstring(L, 2);
	loggers::information(__FUNCTION__, __LINE__, "Loading DLL: %s, Entry: %s", dllPath, entryName);
	HMODULE hModule = LoadLibraryA(dllPath);
	if (hModule)
	{
		FARPROC entryProc = GetProcAddress(hModule, entryName);
		if (entryProc)
		{
			loggers::information(__FUNCTION__, __LINE__, "Calling DLL Entry: %s", entryName);
			typedef void (*EntryFunc)();
			EntryFunc func = reinterpret_cast<EntryFunc>(entryProc);
			func();
			lua_pushboolean(L, true);
			return 1;
		}
		loggers::information(__FUNCTION__, __LINE__, "Failed to get DLL Entry: %s", entryName);
	}
	loggers::information(__FUNCTION__, __LINE__, "Failed to load DLL: %s", dllPath);
	lua_pushboolean(L, false);
	return 1;
}

bool LuaExec::ProcessReturnValues(int32_t nRet)
{
	std::set<const void*> visited;
	for (int32_t i = 1; i <= nRet; ++i)
	{
		if (lua_istable(L, -1))
		{
			CheckPatternMatch();
			CheckMainFunction();
			TableProcess(visited);
			lua_pop(L, 1);
			continue;
		}
		lua_pop(L, 1);
	}
	return true;
}

bool LuaExec::TableProcess(std::set<const void*>& visited)
{
	const void* tablePtr = lua_topointer(L, -1);
	if (visited.find(tablePtr) != visited.end())
	{
		return true;
	}
	visited.insert(tablePtr);

	lua_pushnil(L);
	while (lua_next(L, -2) != 0)
	{
		if (lua_istable(L, -1))
		{
			CheckPatternMatch();
			CheckMainFunction();
			TableProcess(visited);
		}
		lua_pop(L, 1);
	}
	return true;
}

bool LuaExec::CheckMainFunction()
{
	lua_getfield(L, -1, "main");
	if (lua_isfunction(L, -1))
	{
		lua_pushvalue(L, -1);
		if (lua_pcall(L, 0, 0, 0) != LUA_OK)
		{
			const char* errorMsg = lua_tostring(L, -1);
			loggers::information(__FUNCTION__, __LINE__, "Lua Error: %s", errorMsg);
			lua_pop(L, 1);
		}
		lua_pop(L, 1);
		return true;
	}
	lua_pop(L, 1);
	return false;
}

bool LuaExec::CheckPatternMatch()
{
	lua_getfield(L, -1, "pattern");
	const char* pattern = nullptr;
	if (lua_isstring(L, -1))
	{
		pattern = lua_tostring(L, -1);
	}
	lua_getfield(L, -2, "match");
	if (pattern && lua_isfunction(L, -1))
	{
		std::string patternStr(pattern);
		uintptr_t foundAddress = searcher::search<uintptr_t>(".text", patternStr);

		if (foundAddress != 0)
		{
			lua_getglobal(L, "context");
			if (lua_isuserdata(L, -1))
			{
				void* udata = lua_touserdata(L, -1);
				*reinterpret_cast<uintptr_t*>(udata) = foundAddress;
			}
			lua_pop(L, 1);

			lua_pushvalue(L, -1);
			lua_getglobal(L, "context");

			if (lua_pcall(L, 1, 0, 0) != LUA_OK)
			{
				const char* errorMsg = lua_tostring(L, -1);
				loggers::information(__FUNCTION__, __LINE__, "Lua Error: %s", errorMsg);
				lua_pop(L, 1);
			}
		}
		lua_pop(L, 2);
		return true;
	}
	lua_pop(L, 2);
	return false;
}

static int Lua_Sleep(lua_State* L)
{
	int ms = (int)luaL_optinteger(L, 1, 0);
	if (ms <= 0)
	{
		return 0;
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(ms));
	return 0;
}

static void WaitForMainWindow(std::string windowClass)
{
	while (!WindowLoaded(windowClass))
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
}

void LuaOnceWithoutThread(const fs::path LuaPath)
{
	LuaExec luaExec;
	{
		fs::path ScriptFolder = LuaPath / "Runonce";
		ExecuteLua(ScriptFolder);
	}
}

DWORD WINAPI LuaOnceWithThread(const fs::path LuaPath)
{
	LuaExec luaExec;
	{
		fs::path ScriptFolder = LuaPath / "Runonce" / "NewThread";
		ExecuteLua(ScriptFolder);
	}
	return 0;
}

DWORD WINAPI LuaAlwaysWithThread(const fs::path LuaPath)
{
	fs::path ScriptFolder = LuaPath / "Always";
	while (true)
	{
		ExecuteLua(ScriptFolder);
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
	return 0;
}

void ExecuteLua(fs::path LuaFileFolder)
{
	LuaExec luaExec;
	std::error_code ec;
	if (fs::exists(LuaFileFolder, ec) && fs::is_directory(LuaFileFolder, ec))
	{
		for (const auto& entry : fs::directory_iterator(LuaFileFolder, ec))
		{
			if (entry.is_regular_file(ec) && entry.path().extension() == ".lua")
			{
				luaExec.ExecuteFile(entry.path().string().c_str());
			}
		}
	}
}
