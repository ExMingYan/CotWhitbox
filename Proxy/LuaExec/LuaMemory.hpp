#ifndef LUAMEMORY_HPP
#define LUAMEMORY_HPP

#include <Lua/lua.hpp>
#include "utils/ProcessInfo.h"
#include <cstdint>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <windows.h>
#include <array>
#include <utility>

constexpr std::array<std::pair<const char*, DWORD>, 5> MemoryAttributes =
{{
	{"R", PAGE_READONLY},
	{"W", PAGE_READWRITE},
	{"X", PAGE_EXECUTE},
	{"RX", PAGE_EXECUTE_READ},
	{"RWX", PAGE_EXECUTE_READWRITE}
}};

static int Lua_ModuleExists(lua_State* L);
static int Lua_GetAddressModule(lua_State* L);
static int Lua_GetModuleAddress(lua_State* L);
static int Lua_Alloc(lua_State* L);
static int Lua_GetProtect(lua_State* L);
static int Lua_SetProtect(lua_State* L);
static int Lua_Free(lua_State* L);
static int ReadBits(lua_State* L);
static int ReadFloat(lua_State* L);
static int ReadDouble(lua_State* L);
static int ReadPointer(lua_State* L);
static int ReadBytes(lua_State* L);
static int WriteBits(lua_State* L);
static int WriteFloat(lua_State* L);
static int WriteDouble(lua_State* L);
static int WritePointer(lua_State* L);
static int WriteBytes(lua_State* L);
static int PatternSearch(lua_State* L);

static int ContextIndex(lua_State* L);
static int ContextNewIndex(lua_State* L);
static int ContextFunction(lua_State* L);
static int ContextAddress(lua_State* L);
static int ContextInteger(lua_State* L);
static int FunctionCall(lua_State* L);
static int FunctionToString(lua_State* L);
static int AddressIndex(lua_State* L);
static int AddressNewIndex(lua_State* L);
static int AddressAdd(lua_State* L);
static int AddressSub(lua_State* L);
static int AddressEq(lua_State* L);
static int AddressLt(lua_State* L);
static int AddressLe(lua_State* L);
static int AddressToString(lua_State* L);
static int AddressReadBits(lua_State* L);
static int AddressReadFloat(lua_State* L);
static int AddressReadDouble(lua_State* L);
static int AddressWriteBits(lua_State* L);
static int AddressWriteFloat(lua_State* L);
static int AddressWriteDouble(lua_State* L);
static int AddressReadPointer(lua_State* L);
static int AddressWritePointer(lua_State* L);
static int AddressReadBytes(lua_State* L);
static int AddressWriteBytes(lua_State* L);

intptr_t GetAddressFromStack(lua_State* L, int idx);
void PushAddressObject(lua_State* L, intptr_t address);
void PushFunctionObject(lua_State* L, intptr_t address);

template<typename T>
bool ReadMemory(intptr_t address, T &defaultValue)
{
	static_assert(std::is_arithmetic<T>::value, "ReadMemory requires a numeric type (integer or floating point)");
	if (isAddressAccessAble(address))
	{
		defaultValue = *reinterpret_cast<T*>(address);
		return true;
	}
	return false;
}

template<typename T>
bool WriteMemory(intptr_t address, T value)
{
	static_assert(std::is_arithmetic<T>::value, "WriteMemory requires a numeric type (integer or floating point)");
	DWORD oldProctect;
	if (isAddressAccessAble(address) && VirtualProtect((LPVOID)address, sizeof(T), PAGE_EXECUTE_READWRITE, &oldProctect))
	{
		*reinterpret_cast<T*>(address) = value;
		VirtualProtect((LPVOID)address, sizeof(T), oldProctect, &oldProctect);
		return true;
	}
	return false;
}

template<typename T>
int ReadIntegerValue(lua_State* L)
{
	static_assert(std::is_integral<T>::value, "AddressReadIntegerValue requires an integral type");
	intptr_t addr = GetAddressFromStack(L, 1);
	if (isAddressAccessAble(addr))
	{
		T val = T();
		if (ReadMemory(addr, val))
		{
			lua_pushinteger(L, static_cast<lua_Integer>(val));
			return 1;
		}
	}
	lua_pushnil(L);
	return 1;
}

template<typename T>
int WriteIntegerValue(lua_State* L)
{
	static_assert(std::is_integral<T>::value, "AddressWrite requires an integral type");
	intptr_t addr = GetAddressFromStack(L, 1);
	if (lua_isnumber(L, 2))
	{
		T val = static_cast<T>(lua_tointeger(L, 2));
		lua_pushboolean(L, WriteMemory<T>(addr, val));
		return 1;
	}
	lua_pushboolean(L, false);
	return 1;
}

template<typename T>
int AddressReadIntegerValue(lua_State* L)
{
	static_assert(std::is_integral<T>::value, "AddressReadIntegerValue requires an integral type");
	if (lua_isuserdata(L, 1))
	{
		intptr_t addr = *reinterpret_cast<intptr_t*>(lua_touserdata(L, 1));
		if (lua_isnumber(L, 2))
		{
			addr += (ptrdiff_t)lua_tointeger(L, 2);
		}
		if (isAddressAccessAble(addr))
		{
			T val = T();
			if (ReadMemory(addr, val))
			{
				lua_pushinteger(L, static_cast<lua_Integer>(val));
				return 1;
			}
		}
	}
	lua_pushnil(L);
	return 1;
}

template<typename T>
int AddressWrite(lua_State* L)
{
	static_assert(std::is_integral<T>::value, "AddressWrite requires an integral type");
	if (lua_isuserdata(L, 1) && lua_isnumber(L, 2) && lua_isnumber(L, 3))
	{
		intptr_t addr = *reinterpret_cast<intptr_t*>(lua_touserdata(L, 1));
		addr += (ptrdiff_t)lua_tointeger(L, 2);
		T val = static_cast<T>(lua_tointeger(L, 3));
		lua_pushboolean(L, WriteMemory<T>(addr, val));
		return 1;
	}
	lua_pushboolean(L, false);
	return 1;
}

template<std::size_t... I>
static uint64_t InvokeByIndex(intptr_t addr, const uint64_t* argv, std::index_sequence<I...>)
{
	using Fn = uint64_t(*)(decltype(argv[I])...);
	Fn fn = reinterpret_cast<Fn>(addr);
	return fn(argv[I]...);
}

template<std::size_t N>
static uint64_t InvokeN(intptr_t addr, const uint64_t* argv)
{
	return InvokeByIndex(addr, argv, std::make_index_sequence<N>{});
}

#endif // !LUAMEMORY_HPP
