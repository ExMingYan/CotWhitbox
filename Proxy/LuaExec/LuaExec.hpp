#ifndef LUAEXEC_HPP
#define LUAEXEC_HPP

#include <Lua/lua.hpp>
#include <cstdint>
#include <filesystem>
#include <set>
#include <windows.h>

namespace fs = std::filesystem;

fs::path GetLuaScriptFolder();
void LuaOnceWithoutThread(const fs::path LuaPath);
DWORD WINAPI LuaOnceWithThread(const fs::path LuaPath);
DWORD WINAPI LuaAlwaysWithThread(const fs::path LuaPath);
void ExecuteLua(fs::path LuaFileFolder);
static int Lua_CheckGameVersion(lua_State* L);
static int Lua_LoadLibrary(lua_State* L);
static int Lua_LoadLibraryEntry(lua_State* L);
static int Lua_Sleep(lua_State* L);
static void WaitForMainWindow(std::string windowClass);

class LuaExec
{
public:
	LuaExec();
	~LuaExec();
	bool ExecuteFile(const char* filename);
private:
	lua_State* L = nullptr;
	bool ProcessReturnValues(int32_t nRet);
	bool TableProcess(std::set<const void*>& visited);
	bool CheckPatternMatch();
	bool CheckMainFunction();
};

#endif // !LUAEXEC_HPP
