#include "LuaKey.h"
#include <chrono>
#include <map>
#include <string>
#include <thread>
#include <vector>
#include <windows.h>

static int Lua_KeyUp(lua_State* L);
static int Lua_KeyDown(lua_State* L);
static int Lua_PressKey(lua_State* L);
static int Lua_IsKeyPressed(lua_State* L);
static int Lua_SendString(lua_State* L);
static int Lua_GetToggleState(lua_State* L);
static int Lua_BlockInput(lua_State* L);
static void ProcessPressKeyTable(lua_State* L, int tableIndex);
static void PressKeyInternal(int vk, int pressTime, int sleepTime);
static int KeyStringToVK(std::string keyString);
static int GetVKFromLua(lua_State* L, int index);

static const std::map<std::string, int> vkMap =
{
	{"VK_LBUTTON", 0x01}, {"VK_RBUTTON", 0x02}, {"VK_CANCEL", 0x03}, {"VK_MBUTTON", 0x04},
	{"VK_XBUTTON1", 0x05}, {"VK_XBUTTON2", 0x06}, {"VK_BACK", 0x08}, {"VK_TAB", 0x09},
	{"VK_CLEAR", 0x0C}, {"VK_RETURN", 0x0D}, {"VK_SHIFT", 0x10}, {"VK_CONTROL", 0x11},
	{"VK_MENU", 0x12}, {"VK_PAUSE", 0x13}, {"VK_CAPITAL", 0x14}, {"VK_ESCAPE", 0x1B},
	{"VK_SPACE", 0x20}, {"VK_PRIOR", 0x21}, {"VK_NEXT", 0x22}, {"VK_END", 0x23},
	{"VK_HOME", 0x24}, {"VK_LEFT", 0x25}, {"VK_UP", 0x26}, {"VK_RIGHT", 0x27},
	{"VK_DOWN", 0x28}, {"VK_SELECT", 0x29}, {"VK_PRINT", 0x2A}, {"VK_EXECUTE", 0x2B},
	{"VK_SNAPSHOT", 0x2C}, {"VK_INSERT", 0x2D}, {"VK_DELETE", 0x2E}, {"VK_HELP", 0x2F},
	{"VK_0", 0x30}, {"VK_1", 0x31}, {"VK_2", 0x32}, {"VK_3", 0x33}, {"VK_4", 0x34},
	{"VK_5", 0x35}, {"VK_6", 0x36}, {"VK_7", 0x37}, {"VK_8", 0x38}, {"VK_9", 0x39},
	{"VK_A", 0x41}, {"VK_B", 0x42}, {"VK_C", 0x43}, {"VK_D", 0x44}, {"VK_E", 0x45},
	{"VK_F", 0x46}, {"VK_G", 0x47}, {"VK_H", 0x48}, {"VK_I", 0x49}, {"VK_J", 0x4A},
	{"VK_K", 0x4B}, {"VK_L", 0x4C}, {"VK_M", 0x4D}, {"VK_N", 0x4E}, {"VK_O", 0x4F},
	{"VK_P", 0x50}, {"VK_Q", 0x51}, {"VK_R", 0x52}, {"VK_S", 0x53}, {"VK_T", 0x54},
	{"VK_U", 0x55}, {"VK_V", 0x56}, {"VK_W", 0x57}, {"VK_X",0x58}, {"VK_Y", 0x59},
	{"VK_Z", 0x5A}, {"VK_LWIN", 0x5B}, {"VK_RWIN", 0x5C}, {"VK_APPS", 0x5D},
	{"VK_SLEEP", 0x5F}, {"VK_NUMPAD0", 0x60}, {"VK_NUMPAD1", 0x61}, {"VK_NUMPAD2", 0x62},
	{"VK_NUMPAD3", 0x63}, {"VK_NUMPAD4", 0x64}, {"VK_NUMPAD5", 0x65}, {"VK_NUMPAD6", 0x66},
	{"VK_NUMPAD7", 0x67}, {"VK_NUMPAD8", 0x68}, {"VK_NUMPAD9", 0x69}, {"VK_MULTIPLY", 0x6A},
	{"VK_ADD", 0x6B}, {"VK_SEPARATOR", 0x6C}, {"VK_SUBTRACT", 0x6D}, {"VK_DECIMAL", 0x6E},
	{"VK_DIVIDE", 0x6F}, {"VK_F1", 0x70}, {"VK_F2", 0x71}, {"VK_F3", 0x72}, {"VK_F4", 0x73},
	{"VK_F5", 0x74}, {"VK_F6", 0x75}, {"VK_F7", 0x76}, {"VK_F8", 0x77}, {"VK_F9", 0x78},
	{"VK_F10", 0x79}, {"VK_F11", 0x7A}, {"VK_F12", 0x7B}, {"VK_F13", 0x7C}, {"VK_F14", 0x7D},
	{"VK_F15", 0x7E}, {"VK_F16", 0x7F}, {"VK_F17", 0x80}, {"VK_F18", 0x81}, {"VK_F19", 0x82},
	{"VK_F20", 0x83}, {"VK_F21", 0x84}, {"VK_F22", 0x85}, {"VK_F23", 0x86}, {"VK_F24", 0x87},
	{"VK_NUMLOCK", 0x90}, {"VK_SCROLL", 0x91}, {"VK_LSHIFT", 0xA0}, {"VK_RSHIFT", 0xA1},
	{"VK_LCONTROL", 0xA2}, {"VK_RCONTROL", 0xA3}, {"VK_LMENU", 0xA4}, {"VK_RMENU", 0xA5}
};

void InitKeyFunctions(lua_State* L)
{
	lua_register(L, "KeyUp", Lua_KeyUp);
	lua_register(L, "KeyDown", Lua_KeyDown);
	lua_register(L, "PressKey", Lua_PressKey);
	lua_register(L, "IsKeyPressed", Lua_IsKeyPressed);
	lua_register(L, "SendString", Lua_SendString);
	lua_register(L, "GetToggleState", Lua_GetToggleState);
	lua_register(L, "BlockInput", Lua_BlockInput);
}

static int Lua_KeyUp(lua_State* L)
{
	int vk = GetVKFromLua(L, 1);
	if (vk == 0)
	{
		return luaL_argerror(L, 1, "invalid key code or key string");
	}
	INPUT input = { 0 };
	input.type = INPUT_KEYBOARD;
	input.ki.wVk = vk;
	input.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &input, sizeof(INPUT));
	return 0;
}

static int Lua_KeyDown(lua_State* L)
{
	int vk = GetVKFromLua(L, 1);
	if (vk == 0)
	{
		return luaL_argerror(L, 1, "invalid key code or key string");
	}
	INPUT input = { 0 };
	input.type = INPUT_KEYBOARD;
	input.ki.wVk = vk;
	SendInput(1, &input, sizeof(INPUT));
	return 0;
}

static int Lua_PressKey(lua_State* L)
{
	if (lua_istable(L, 1))
	{
		ProcessPressKeyTable(L, 1);
	}
	else
	{
		int vk = GetVKFromLua(L, 1);
		if (vk == 0)
		{
			return luaL_argerror(L, 1, "invalid key code or key string");
		}
		int pressTime = (int)luaL_optinteger(L, 2, 16);
		int sleepTime = (int)luaL_optinteger(L, 3, 0);
		PressKeyInternal(vk, pressTime, sleepTime);
	}
	return 0;
}

static int Lua_IsKeyPressed(lua_State* L)
{
	int vk = GetVKFromLua(L, 1);
	if (vk == 0)
	{
		return luaL_argerror(L, 1, "invalid key code or key string");
	}
	bool isPressed = (GetAsyncKeyState(vk) & 0x8000) != 0;
	lua_pushboolean(L, isPressed);
	return 1;
}

static int Lua_SendString(lua_State* L)
{
	const char* str = luaL_checkstring(L, 1);
	if (str == nullptr)
	{
		return 0;
	}

	std::vector<INPUT> inputs;
	for (int i = 0; str[i] != '\0'; ++i)
	{
		INPUT input_down = { 0 };
		input_down.type = INPUT_KEYBOARD;
		input_down.ki.wScan = str[i];
		input_down.ki.dwFlags = KEYEVENTF_UNICODE;
		inputs.push_back(input_down);

		INPUT input_up = { 0 };
		input_up.type = INPUT_KEYBOARD;
		input_up.ki.wScan = str[i];
		input_up.ki.dwFlags = KEYEVENTF_UNICODE | KEYEVENTF_KEYUP;
		inputs.push_back(input_up);
	}

	if (!inputs.empty())
	{
		SendInput((UINT)inputs.size(), inputs.data(), sizeof(INPUT));
	}

	return 0;
}

static int Lua_GetToggleState(lua_State* L)
{
	int vk = GetVKFromLua(L, 1);
	if (vk == 0)
	{
		return luaL_argerror(L, 1, "invalid key code or key string");
	}
	bool isToggled = (GetKeyState(vk) & 0x0001) != 0;
	lua_pushboolean(L, isToggled);
	return 1;
}

static int Lua_BlockInput(lua_State* L)
{
	bool block = lua_toboolean(L, 1);
	if (BlockInput(block))
	{
		lua_pushboolean(L, true);
	}
	else
	{
		lua_pushboolean(L, false);
	}
	return 1;
}

static void ProcessPressKeyTable(lua_State* L, int tableIndex)
{
	int absTableIndex = lua_absindex(L, tableIndex);

	lua_rawgeti(L, absTableIndex, 1);
	bool isTableOfTables = lua_istable(L, -1);
	lua_pop(L, 1);

	if (isTableOfTables)
	{
		lua_pushnil(L);
		while (lua_next(L, absTableIndex) != 0)
		{
			if (lua_istable(L, -1))
			{
				ProcessPressKeyTable(L, -1);
			}
			lua_pop(L, 1);
		}
	}
	else
	{
		lua_getfield(L, absTableIndex, "vk");
		int vk = GetVKFromLua(L, -1);
		lua_pop(L, 1);

		if (vk != 0)
		{
			lua_getfield(L, absTableIndex, "pressTime");
			int pressTime = (int)luaL_optinteger(L, -1, 16);
			lua_pop(L, 1);
			lua_getfield(L, absTableIndex, "sleepTime");
			int sleepTime = (int)luaL_optinteger(L, -1, 0);
			lua_pop(L, 1);
			PressKeyInternal(vk, pressTime, sleepTime);
		}
		else
		{
			lua_rawgeti(L, absTableIndex, 1);
			vk = GetVKFromLua(L, -1);
			lua_pop(L, 1);

			if (vk != 0)
			{
				lua_rawgeti(L, absTableIndex, 2);
				int pressTime = (int)luaL_optinteger(L, -1, 16);
				lua_pop(L, 1);
				lua_rawgeti(L, absTableIndex, 3);
				int sleepTime = (int)luaL_optinteger(L, -1, 0);
				lua_pop(L, 1);
				PressKeyInternal(vk, pressTime, sleepTime);
			}
		}
	}
}

static void PressKeyInternal(int vk, int pressTime, int sleepTime)
{
	INPUT input_down = { 0 };
	input_down.type = INPUT_KEYBOARD;
	input_down.ki.wVk = vk;
	SendInput(1, &input_down, sizeof(INPUT));

	if (pressTime > 0)
	{
		std::thread([vk, pressTime]() {
			std::this_thread::sleep_for(std::chrono::milliseconds(pressTime));
			INPUT input_up = { 0 };
			input_up.type = INPUT_KEYBOARD;
			input_up.ki.wVk = vk;
			input_up.ki.dwFlags = KEYEVENTF_KEYUP;
			SendInput(1, &input_up, sizeof(INPUT));
			}).detach();
	}
	else
	{
		INPUT input_up = { 0 };
		input_up.type = INPUT_KEYBOARD;
		input_up.ki.wVk = vk;
		input_up.ki.dwFlags = KEYEVENTF_KEYUP;
		SendInput(1, &input_up, sizeof(INPUT));
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime));
}

static int GetVKFromLua(lua_State* L, int index)
{
	if (lua_isinteger(L, index))
	{
		return (int)lua_tointeger(L, index);
	}
	if (lua_isstring(L, index))
	{
		const char* keyString = lua_tostring(L, index);
		return KeyStringToVK(keyString);
	}
	return 0;
}

static int KeyStringToVK(std::string keyString)
{
	auto it = vkMap.find(keyString);
	if (it != vkMap.end())
	{
		return it->second;
	}
	return 0;
}
