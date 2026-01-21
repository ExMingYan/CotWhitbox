#include "LuaMemory.hpp"
#include "LuaMemory.h"
#include "utils/searcher.h"

static const char* ContextAccess = "ContextAccess";
static const char* AddressMeta = "AddressMeta";
static const char* FunctionMeta = "FunctionMeta";

void InitMemoryAccess(lua_State* L)
{
	lua_register(L, "AllocMemory", Lua_Alloc);
	lua_register(L, "GetMemoryProtect", Lua_GetProtect);
	lua_register(L, "SetMemoryProtect", Lua_SetProtect);
	lua_register(L, "FreeMemory", Lua_Free);
	lua_register(L, "readInt8", ReadIntegerValue<int8_t>);
	lua_register(L, "readUInt8", ReadIntegerValue<uint8_t>);
	lua_register(L, "readByte", ReadIntegerValue<uint8_t>);
	lua_register(L, "readInt16", ReadIntegerValue<int16_t>);
	lua_register(L, "readUInt16", ReadIntegerValue<uint16_t>);
	lua_register(L, "readInt32", ReadIntegerValue<int32_t>);
	lua_register(L, "readUInt32", ReadIntegerValue<uint32_t>);
	lua_register(L, "readInt64", ReadIntegerValue<int64_t>);
	lua_register(L, "readFloat", ReadFloat);
	lua_register(L, "readDouble", ReadDouble);
	lua_register(L, "readPointer", ReadPointer);
	lua_register(L, "readBytes", ReadBytes);
	lua_register(L, "writeInt8", WriteIntegerValue<int8_t>);
	lua_register(L, "writeByte", WriteIntegerValue<uint8_t>);
	lua_register(L, "writeUInt8", WriteIntegerValue<uint8_t>);
	lua_register(L, "writeInt16", WriteIntegerValue<int16_t>);
	lua_register(L, "writeUInt16", WriteIntegerValue<uint16_t>);
	lua_register(L, "writeInt32", WriteIntegerValue<int32_t>);
	lua_register(L, "writeUInt32", WriteIntegerValue<uint32_t>);
	lua_register(L, "writeInt64", WriteIntegerValue<int64_t>);
	lua_register(L, "writeFloat", WriteFloat);
	lua_register(L, "writeDouble", WriteDouble);
	lua_register(L, "writePointer", WritePointer);
	lua_register(L, "writeBytes", WriteBytes);
	lua_register(L, "AOBScanModule", PatternSearch);

	luaL_newmetatable(L, ContextAccess);
	lua_pushcfunction(L, ContextIndex);
	lua_setfield(L, -2, "__index");
	lua_pushcfunction(L, ContextNewIndex);
	lua_setfield(L, -2, "__newindex");
	lua_pop(L, 1);

	luaL_newmetatable(L, FunctionMeta);
	lua_pushcfunction(L, FunctionCall);
	lua_setfield(L, -2, "__call");
	lua_pushcfunction(L, FunctionToString);
	lua_setfield(L, -2, "__tostring");
	lua_pop(L, 1);

	luaL_newmetatable(L, AddressMeta);
	lua_pushcfunction(L, AddressIndex);
	lua_setfield(L, -2, "__index");
	lua_pushcfunction(L, AddressNewIndex);
	lua_setfield(L, -2, "__newindex");
	lua_pushcfunction(L, AddressAdd);
	lua_setfield(L, -2, "__add");
	lua_pushcfunction(L, AddressSub);
	lua_setfield(L, -2, "__sub");

	lua_pushcfunction(L, AddressEq);
	lua_setfield(L, -2, "__eq");
	lua_pushcfunction(L, AddressLt);
	lua_setfield(L, -2, "__lt");
	lua_pushcfunction(L, AddressLe);
	lua_setfield(L, -2, "__le");

	lua_pushcfunction(L, AddressToString);
	lua_setfield(L, -2, "__tostring");

	lua_pushcfunction(L, AddressReadIntegerValue<int8_t>);
	lua_setfield(L, -2, "readInt8");
	lua_pushcfunction(L, AddressReadIntegerValue<uint8_t>);
	lua_setfield(L, -2, "readUInt8");
	lua_pushcfunction(L, AddressReadIntegerValue<uint8_t>);
	lua_setfield(L, -2, "readByte");
	lua_pushcfunction(L, AddressReadIntegerValue<int16_t>);
	lua_setfield(L, -2, "readInt16");
	lua_pushcfunction(L, AddressReadIntegerValue<uint16_t>);
	lua_setfield(L, -2, "readUInt16");
	lua_pushcfunction(L, AddressReadIntegerValue<int32_t>);
	lua_setfield(L, -2, "readInt32");
	lua_pushcfunction(L, AddressReadIntegerValue<uint32_t>);
	lua_setfield(L, -2, "readUInt32");
	lua_pushcfunction(L, AddressReadIntegerValue<int64_t>);
	lua_setfield(L, -2, "readInt64");
	lua_pushcfunction(L, AddressReadFloat);
	lua_setfield(L, -2, "readFloat");
	lua_pushcfunction(L, AddressReadDouble);
	lua_setfield(L, -2, "readDouble");
	lua_pushcfunction(L, AddressReadPointer);
	lua_setfield(L, -2, "readPointer");
	lua_pushcfunction(L, AddressReadBytes);
	lua_setfield(L, -2, "readBytes");
	lua_pushcfunction(L, AddressWrite<int8_t>);
	lua_setfield(L, -2, "writeInt8");
	lua_pushcfunction(L, AddressWrite<uint8_t>);
	lua_setfield(L, -2, "writeUInt8");
	lua_pushcfunction(L, AddressWrite<uint8_t>);
	lua_setfield(L, -2, "writeByte");
	lua_pushcfunction(L, AddressWrite<int16_t>);
	lua_setfield(L, -2, "writeInt16");
	lua_pushcfunction(L, AddressWrite<uint16_t>);
	lua_setfield(L, -2, "writeUInt16");
	lua_pushcfunction(L, AddressWrite<int32_t>);
	lua_setfield(L, -2, "writeInt32");
	lua_pushcfunction(L, AddressWrite<uint32_t>);
	lua_setfield(L, -2, "writeUInt32");
	lua_pushcfunction(L, AddressWrite<int64_t>);
	lua_setfield(L, -2, "writeInt64");
	lua_pushcfunction(L, AddressWriteFloat);
	lua_setfield(L, -2, "writeFloat");
	lua_pushcfunction(L, AddressWriteDouble);
	lua_setfield(L, -2, "writeDouble");
	lua_pushcfunction(L, AddressWritePointer);
	lua_setfield(L, -2, "writePointer");
	lua_pushcfunction(L, AddressWriteBytes);
	lua_setfield(L, -2, "writeBytes");

	lua_pop(L, 1);

	void* udata = lua_newuserdata(L, sizeof(void*));
	*reinterpret_cast<intptr_t*>(udata) = 0;
	luaL_getmetatable(L, ContextAccess);
	lua_setmetatable(L, -2);
	lua_setglobal(L, "context");
}

static int Lua_Alloc(lua_State* L)
{
	int64_t size = luaL_checkinteger(L, 1);
	if (size <= 0)
	{
		lua_pushnil(L);
		return 1;
	}
	const char* attribute = luaL_checkstring(L, 2);
	std::string attrStr(attribute ? attribute : MemoryAttributes[4].first);
	DWORD protect = PAGE_EXECUTE_READWRITE;
	for (const auto& pair : MemoryAttributes)
	{
		if (attrStr == pair.first)
		{
			protect = pair.second;
			break;
		}
	}
	LPVOID addr = VirtualAlloc(NULL, (SIZE_T)size, MEM_COMMIT | MEM_RESERVE, protect);
	if (addr)
	{
		PushAddressObject(L, (intptr_t)addr);
		return 1;
	}
	lua_pushnil(L);
	return 1;
}

static int Lua_GetProtect(lua_State* L)
{
	intptr_t addr = GetAddressFromStack(L, 1);
	if (isAddressAccessAble(addr))
	{
		DWORD protect = GetAddressProtect(addr);
		if (protect != 0)
		{
			for (const auto& pair : MemoryAttributes)
			{
				if (pair.second == protect)
				{
					lua_pushstring(L, pair.first);
					return 1;
				}
			}
		}
	}
	lua_pushnil(L);
	return 1;
}

static int Lua_SetProtect(lua_State* L)
{
	intptr_t addr = GetAddressFromStack(L, 1);
	size_t size = (size_t)luaL_checkinteger(L, 2);
	const char* attribute = luaL_checkstring(L, 3);
	std::string attrStr(attribute ? attribute : "RWX");
	if (isAddressAccessAble(addr) && size > 0)
	{
		DWORD protect = PAGE_EXECUTE_READWRITE;
		for (const auto& pair : MemoryAttributes)
		{
			if (attrStr == pair.first)
			{
				protect = pair.second;
				break;
			}
		}
		DWORD oldProtect;
		BOOL result = VirtualProtect((LPVOID)addr, (SIZE_T)size, protect, &oldProtect);
		lua_pushboolean(L, result != 0);
		return 1;
	}
	lua_pushboolean(L, false);
	return 1;
}

static int Lua_Free(lua_State* L)
{
	intptr_t addr = GetAddressFromStack(L, 1);
	if (isAddressAccessAble(addr))
	{
		BOOL result = VirtualFree((LPVOID)addr, 0, MEM_RELEASE);
		lua_pushboolean(L, result != 0);
		return 1;
	}
	lua_pushboolean(L, false);
	return 1;
}

static int ReadFloat(lua_State* L)
{
	intptr_t addr = GetAddressFromStack(L, 1);
	if (isAddressAccessAble(addr))
	{
		float val = 0.0f;
		if (ReadMemory(addr, val))
		{
			lua_pushnumber(L, val);
			return 1;
		}
	}
	lua_pushnil(L);
	return 1;
}

static int WriteFloat(lua_State* L)
{
	intptr_t addr = GetAddressFromStack(L, 1);
	if (lua_isnumber(L, 2))
	{
		float val = static_cast<float>(lua_tonumber(L, 2));
		lua_pushboolean(L, WriteMemory(addr, val));
		return 1;
	}
	lua_pushboolean(L, false);
	return 1;
}

static int ReadDouble(lua_State* L)
{
	intptr_t addr = GetAddressFromStack(L, 1);
	if (isAddressAccessAble(addr))
	{
		double val = 0.0;
		if (ReadMemory(addr, val))
		{
			lua_pushnumber(L, val);
			return 1;
		}
	}
	lua_pushnil(L);
	return 1;
}

static int WriteDouble(lua_State* L)
{
	intptr_t addr = GetAddressFromStack(L, 1);
	if (lua_isnumber(L, 2))
	{
		double val = static_cast<double>(lua_tonumber(L, 2));
		lua_pushboolean(L, WriteMemory(addr, val));
		return 1;
	}
	lua_pushboolean(L, false);
	return 1;
}

static int ReadPointer(lua_State* L)
{
	intptr_t addr = GetAddressFromStack(L, 1);
	if (isAddressAccessAble(addr))
	{
		intptr_t val = 0;
		if (ReadMemory(addr, val))
		{
			PushAddressObject(L, val);
			return 1;
		}
	}
	lua_pushnil(L);
	return 1;
}

static int WritePointer(lua_State* L)
{
	intptr_t addr = GetAddressFromStack(L, 1);
	if (isAddressAccessAble(addr))
	{
		intptr_t val = GetAddressFromStack(L, 2);
		lua_pushboolean(L, WriteMemory(addr, val));
		return 1;
	}
	lua_pushboolean(L, false);
	return 1;
}

static int ReadBytes(lua_State* L)
{
	intptr_t addr = GetAddressFromStack(L, 1);
	size_t offset = 0;
	size_t length = 0;

	if (lua_isnumber(L, 2) && lua_isnumber(L, 3))
	{
		offset = (size_t)lua_tointeger(L, 2);
		length = (size_t)luaL_checkinteger(L, 3);
	}
	else
	{
		length = (size_t)luaL_checkinteger(L, 2);
	}

	if (isAddressAccessAble(addr) && length > 0)
	{
		lua_newtable(L);
		for (size_t i = 0; i < length; ++i)
		{
			intptr_t current_addr = addr + offset + i;
			if (isAddressAccessAble(current_addr))
			{
				uint8_t val = 0;
				if (ReadMemory(current_addr, val))
				{
					lua_pushinteger(L, i + 1);
					lua_pushinteger(L, val);
					lua_settable(L, -3);
				}
				else
				{
					lua_pop(L, 1);
					lua_pushnil(L);
					return 1;
				}
			}
			else
			{
				return 1;
			}
		}
		return 1;
	}
	lua_newtable(L);
	return 1;
}

static int WriteBytes(lua_State* L)
{
	intptr_t addr = GetAddressFromStack(L, 1);
	size_t offset = 0;
	int table_idx = 2;
	if (lua_isnumber(L, 2))
	{
		offset = (size_t)lua_tointeger(L, 2);
		table_idx = 3;
	}

	if (lua_istable(L, table_idx))
	{
		size_t length = lua_rawlen(L, table_idx);
		for (size_t i = 0; i < length; ++i)
		{
			lua_pushinteger(L, i + 1);
			lua_gettable(L, table_idx);
			if (lua_isnumber(L, -1))
			{
				uint8_t val = static_cast<uint8_t>(lua_tointeger(L, -1));
				intptr_t current_addr = addr + offset + i;
				WriteMemory(current_addr, val);
			}
			lua_pop(L, 1);
		}
	}
	return 0;
}

static int PatternSearch(lua_State* L)
{
	if (lua_gettop(L) < 3)
	{
		lua_pushnil(L);
		return 1;
	}
	const char* moduleName = luaL_checkstring(L, 1);
	const char* sectionName = luaL_checkstring(L, 2);
	const char* pattern = luaL_checkstring(L, 3);
	intptr_t foundAddress = searcher::search(moduleName, sectionName, pattern);
	if (foundAddress != 0 && isAddressAccessAble(foundAddress))
	{
		PushAddressObject(L, foundAddress);
		return 1;
	}
	lua_pushnil(L);
	return 1;
}

static int ContextIndex(lua_State* L)
{
	if (lua_isstring(L, 2))
	{
		const char* key = lua_tostring(L, 2);
		if (strcmp(key, "address") == 0)
		{
			lua_pushcfunction(L, ContextAddress);
			return 1;
		}
		if (strcmp(key, "integer") == 0)
		{
			lua_pushcfunction(L, ContextInteger);
			return 1;
		}
		if (strcmp(key, "Function") == 0)
		{
			lua_pushcfunction(L, ContextFunction);
			return 1;
		}
	}

	if (lua_isnumber(L, 2))
	{
		intptr_t address = static_cast<intptr_t>(lua_tointeger(L, 2));
		if (isAddressAccessAble(address))
		{
			uint8_t val = 0;
			if (ReadMemory(address, val))
			{
				lua_pushinteger(L, val);
				return 1;
			}
		}
	}

	if (lua_isuserdata(L, 2))
	{
		intptr_t address = *reinterpret_cast<intptr_t*>(lua_touserdata(L, 2));
		if (isAddressAccessAble(address))
		{
			uint8_t val = 0;
			if (ReadMemory(address, val))
			{
				lua_pushinteger(L, val);
				return 1;
			}
		}
	}
	lua_pushnil(L);
	return 1;
}

static int ContextNewIndex(lua_State* L)
{
	if (lua_isnumber(L, 2) && lua_isnumber(L, 3))
	{
		intptr_t address = static_cast<intptr_t>(lua_tointeger(L, 2));
		uint8_t val = static_cast<uint8_t>(lua_tointeger(L, 3));
		WriteMemory(address, val);
	}

	if (lua_isuserdata(L, 2) && lua_isnumber(L, 3))
	{
		intptr_t address = *reinterpret_cast<intptr_t*>(lua_touserdata(L, 2));
		uint8_t val = static_cast<uint8_t>(lua_tointeger(L, 3));
		WriteMemory(address, val);
	}

	return 0;
}

static int ContextFunction(lua_State* L)
{
	if (lua_gettop(L) < 2)
	{
		lua_pushnil(L);
		return 1;
	}

	intptr_t addr = GetAddressFromStack(L, 2);
	if (isAddressAccessAble(addr))
	{
		PushFunctionObject(L, addr);
		return 1;
	}

	lua_pushnil(L);
	return 1;
}

static int ContextAddress(lua_State* L)
{
	int top = lua_gettop(L);
	if (top >= 2)
	{
		intptr_t address = GetAddressFromStack(L, 2);
		if (!isAddressAccessAble(address))
		{
			lua_pushnil(L);
			return 1;
		}
		PushAddressObject(L, address);
		return 1;
	}
	else
	{
		if (lua_isuserdata(L, 1))
		{
			void* udata = lua_touserdata(L, 1);
			if (udata)
			{
				intptr_t address = *reinterpret_cast<intptr_t*>(udata);
				if (!isAddressAccessAble(address))
				{
					lua_pushnil(L);
					return 1;
				}
				PushAddressObject(L, address);
				return 1;
			}
		}
	}
	return 0;
}

static int ContextInteger(lua_State* L)
{
	int top = lua_gettop(L);
	if (top >= 2)
	{
		intptr_t address = GetAddressFromStack(L, 2);
		lua_pushinteger(L, address);
		return 1;
	}
	return 0;
}

static int FunctionCall(lua_State* L)
{
	if (!lua_isuserdata(L, 1))
	{
		lua_pushnil(L);
		return 1;
	}

	intptr_t addr = *reinterpret_cast<intptr_t*>(lua_touserdata(L, 1));
	if (!isAddressAccessAble(addr))
	{
		lua_pushnil(L);
		return 1;
	}

	int top = lua_gettop(L);
	int argCount = top - 1;
	if (argCount < -1 || argCount > 16)
	{
		lua_pushnil(L);
		return 1;
	}
	uint64_t argv[16] = { 0 };
	for (int i = 0; i < argCount; ++i)
	{
		argv[i] = static_cast<uint64_t>(lua_tointeger(L, i + 2));
	}
	uint64_t result = 0;
	switch (argCount)
	{
		case 0: result = InvokeN<0>(addr, argv); break;
		case 1: result = InvokeN<1>(addr, argv); break;
		case 2: result = InvokeN<2>(addr, argv); break;
		case 3: result = InvokeN<3>(addr, argv); break;
		case 4: result = InvokeN<4>(addr, argv); break;
		case 5: result = InvokeN<5>(addr, argv); break;
		case 6: result = InvokeN<6>(addr, argv); break;
		case 7: result = InvokeN<7>(addr, argv); break;
		case 8: result = InvokeN<8>(addr, argv); break;
		case 9: result = InvokeN<9>(addr, argv); break;
		case 10: result = InvokeN<10>(addr, argv); break;
		case 11: result = InvokeN<11>(addr, argv); break;
		case 12: result = InvokeN<12>(addr, argv); break;
		case 13: result = InvokeN<13>(addr, argv); break;
		case 14: result = InvokeN<14>(addr, argv); break;
		case 15: result = InvokeN<15>(addr, argv); break;
		case 16: result = InvokeN<16>(addr, argv); break;
		default: result = InvokeN<0>(addr, argv); break;
	}
	lua_pushinteger(L, static_cast<lua_Integer>(result));
	return 1;
}

static int FunctionToString(lua_State* L)
{
	if (lua_isuserdata(L, 1))
	{
		intptr_t addr = *reinterpret_cast<intptr_t*>(lua_touserdata(L, 1));
		lua_pushfstring(L, "Function Address: 0x%p", (void*)addr);
		return 1;
	}
	return 0;
}

static int AddressIndex(lua_State* L)
{
	if (lua_isnumber(L, 2))
	{
		intptr_t baseAddr = 0;
		if (lua_isuserdata(L, 1))
			baseAddr = *reinterpret_cast<intptr_t*>(lua_touserdata(L, 1));

		ptrdiff_t offset = (ptrdiff_t)lua_tointeger(L, 2);
		intptr_t targetAddr = baseAddr + offset;

		if (isAddressAccessAble(targetAddr))
		{
			uint8_t val = 0;
			if (ReadMemory(targetAddr, val))
			{
				lua_pushinteger(L, val);
				return 1;
			}
		}
	}

	if (lua_isstring(L, 2))
	{
		lua_getmetatable(L, 1);
		lua_pushvalue(L, 2);
		lua_rawget(L, -2);
		if (!lua_isnil(L, -1))
		{
			return 1;
		}
		lua_pop(L, 2);
	}
	lua_pushnil(L);
	return 1;
}

static int AddressNewIndex(lua_State* L)
{
	if (lua_isnumber(L, 2) && lua_isnumber(L, 3))
	{
		intptr_t baseAddr = 0;
		if (lua_isuserdata(L, 1))
			baseAddr = *reinterpret_cast<intptr_t*>(lua_touserdata(L, 1));

		ptrdiff_t offset = (ptrdiff_t)lua_tointeger(L, 2);
		uint8_t val = (uint8_t)lua_tointeger(L, 3);
		intptr_t targetAddr = baseAddr + offset;
		WriteMemory(targetAddr, val);
	}
	return 0;
}

intptr_t GetAddressFromStack(lua_State* L, int idx)
{
	if (lua_isuserdata(L, idx))
		return *reinterpret_cast<intptr_t*>(lua_touserdata(L, idx));
	if (lua_isnumber(L, idx))
		return (intptr_t)lua_tointeger(L, idx);
	return 0;
}

static int AddressAdd(lua_State* L)
{
	intptr_t val1 = GetAddressFromStack(L, 1);
	intptr_t val2 = GetAddressFromStack(L, 2);
	PushAddressObject(L, val1 + val2);
	return 1;
}

static int AddressSub(lua_State* L)
{
	intptr_t val1 = GetAddressFromStack(L, 1);
	intptr_t val2 = GetAddressFromStack(L, 2);
	PushAddressObject(L, val1 - val2);
	return 1;
}

static int AddressEq(lua_State* L)
{
	intptr_t val1 = GetAddressFromStack(L, 1);
	intptr_t val2 = GetAddressFromStack(L, 2);
	lua_pushboolean(L, val1 == val2);
	return 1;
}

static int AddressLt(lua_State* L)
{
	intptr_t val1 = GetAddressFromStack(L, 1);
	intptr_t val2 = GetAddressFromStack(L, 2);
	lua_pushboolean(L, val1 < val2);
	return 1;
}

static int AddressLe(lua_State* L)
{
	intptr_t val1 = GetAddressFromStack(L, 1);
	intptr_t val2 = GetAddressFromStack(L, 2);
	lua_pushboolean(L, val1 <= val2);
	return 1;
}

static int AddressToString(lua_State* L)
{
	if (lua_isuserdata(L, 1))
	{
		intptr_t addr = *reinterpret_cast<intptr_t*>(lua_touserdata(L, 1));
		lua_pushfstring(L, "Address: 0x%p", (void*)addr);
		return 1;
	}
	return 0;
}

static int AddressReadFloat(lua_State* L)
{
	if (lua_isuserdata(L, 1))
	{
		intptr_t addr = *reinterpret_cast<intptr_t*>(lua_touserdata(L, 1));
		if (lua_isnumber(L, 2))
		{
			addr += (ptrdiff_t)lua_tointeger(L, 2);
		}
		if (isAddressAccessAble(addr))
		{
			float val = 0.0f;
			if (ReadMemory(addr, val))
			{
				lua_pushnumber(L, val);
				return 1;
			}
		}
	}
	lua_pushnil(L);
	return 1;
}

static int AddressWriteFloat(lua_State* L)
{
	if (lua_isuserdata(L, 1) && lua_isnumber(L, 2) && lua_isnumber(L, 3))
	{
		intptr_t addr = *reinterpret_cast<intptr_t*>(lua_touserdata(L, 1));
		addr += (ptrdiff_t)lua_tointeger(L, 2);
		float val = static_cast<float>(lua_tonumber(L, 3));
		WriteMemory(addr, val);
	}
	return 0;
}

static int AddressReadDouble(lua_State* L)
{
	if (lua_isuserdata(L, 1))
	{
		intptr_t addr = *reinterpret_cast<intptr_t*>(lua_touserdata(L, 1));
		if (lua_isnumber(L, 2))
		{
			addr += (ptrdiff_t)lua_tointeger(L, 2);
		}
		if (isAddressAccessAble(addr))
		{
			double val = 0.0;
			if (ReadMemory(addr, val))
			{
				lua_pushnumber(L, val);
				return 1;
			}
		}
	}
	lua_pushnil(L);
	return 1;
}

static int AddressWriteDouble(lua_State* L)
{
	if (lua_isuserdata(L, 1) && lua_isnumber(L, 2) && lua_isnumber(L, 3))
	{
		intptr_t addr = *reinterpret_cast<intptr_t*>(lua_touserdata(L, 1));
		addr += (ptrdiff_t)lua_tointeger(L, 2);
		double val = static_cast<double>(lua_tonumber(L, 3));
		WriteMemory(addr, val);
	}
	return 0;
}

static int AddressReadPointer(lua_State* L)
{
	if (lua_isuserdata(L, 1))
	{
		intptr_t addr = *reinterpret_cast<intptr_t*>(lua_touserdata(L, 1));
		if (lua_isnumber(L, 2))
		{
			addr += (ptrdiff_t)lua_tointeger(L, 2);
		}
		if (isAddressAccessAble(addr))
		{
			intptr_t val = 0;
			if (ReadMemory(addr, val))
			{
				PushAddressObject(L, val);
				return 1;
			}
		}
	}
	lua_pushnil(L);
	return 1;
}

static int AddressWritePointer(lua_State* L)
{
	if (lua_isuserdata(L, 1) && lua_isnumber(L, 2) && lua_isuserdata(L, 3))
	{
		intptr_t addr = *reinterpret_cast<intptr_t*>(lua_touserdata(L, 1));
		addr += (ptrdiff_t)lua_tointeger(L, 2);
		intptr_t val = *reinterpret_cast<intptr_t*>(lua_touserdata(L, 3));
		WriteMemory(addr, val);
	}
	return 0;
}

static int AddressReadBytes(lua_State* L)
{
	intptr_t addr = 0;
	ptrdiff_t offset = 0;
	size_t length = 0;
	int top = lua_gettop(L);

	if (!lua_isuserdata(L, 1))
	{
		return 0;
	}

	addr = *reinterpret_cast<intptr_t*>(lua_touserdata(L, 1));

	if (top == 3 && lua_isnumber(L, 2) && lua_isnumber(L, 3))
	{
		offset = (ptrdiff_t)lua_tointeger(L, 2);
		length = (size_t)lua_tointeger(L, 3);
	}
	else if (top == 2 && lua_isnumber(L, 2))
	{
		offset = 0;
		length = (size_t)lua_tointeger(L, 2);
	}
	else
	{
		return 0;
	}

	addr += offset;

	if (length == 0)
	{
		lua_newtable(L);
		return 1;
	}

	lua_newtable(L);
	for (size_t i = 0; i < length; ++i)
	{
		intptr_t current_addr = addr + i;
		if (isAddressAccessAble(current_addr))
		{
			uint8_t val = 0;
			ReadMemory(current_addr, val);
			lua_pushinteger(L, i + 1);
			lua_pushinteger(L, val);
			lua_settable(L, -3);
		}
		else
		{
			return 1;
		}
	}
	return 1;
}

static int AddressWriteBytes(lua_State* L)
{
	if (lua_isuserdata(L, 1))
	{
		intptr_t addr = *reinterpret_cast<intptr_t*>(lua_touserdata(L, 1));
		ptrdiff_t offset = 0;
		int table_idx = 0;

		if (lua_isnumber(L, 2) && lua_istable(L, 3))
		{
			offset = (ptrdiff_t)lua_tointeger(L, 2);
			table_idx = 3;
		}
		else if (lua_istable(L, 2))
		{
			table_idx = 2;
		}

		if (table_idx != 0)
		{
			addr += offset;
			size_t length = lua_rawlen(L, table_idx);
			for (size_t i = 0; i < length; ++i)
			{
				lua_pushinteger(L, i + 1);
				lua_gettable(L, table_idx);
				if (lua_isnumber(L, -1))
				{
					uint8_t val = static_cast<uint8_t>(lua_tointeger(L, -1));
					intptr_t current_addr = addr + i;
					WriteMemory(current_addr, val);
				}
				lua_pop(L, 1);
			}
		}
	}
	return 0;
}

void PushAddressObject(lua_State* L, intptr_t address)
{
	void* udata = lua_newuserdata(L, sizeof(intptr_t));
	*reinterpret_cast<intptr_t*>(udata) = address;
	luaL_getmetatable(L, AddressMeta);
	lua_setmetatable(L, -2);
}

void PushFunctionObject(lua_State* L, intptr_t address)
{
	void* udata = lua_newuserdata(L, sizeof(intptr_t));
	*reinterpret_cast<intptr_t*>(udata) = address;
	luaL_getmetatable(L, FunctionMeta);
	lua_setmetatable(L, -2);
}
