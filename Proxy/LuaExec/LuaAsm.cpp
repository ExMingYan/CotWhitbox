#include "LuaAsm.h"
#include "asmjit/x86.h"
#include "asmtk/asmparser.h"

static int LuaCompileAsm(lua_State* L);

void InitLuaAsmFunction(lua_State* L)
{
	lua_register(L, "asm", LuaCompileAsm);
}

static int LuaCompileAsm(lua_State* L)
{
	const char* arch_str = nullptr;
	const char* asm_code = nullptr;

	int32_t n = lua_gettop(L);

	if (n == 1 && lua_istable(L, 1))
	{
		lua_getfield(L, 1, "arch");
		if (!lua_isstring(L, -1))
		{
			return luaL_error(L, "expected a 'arch' string field in the table argument");
		}
		arch_str = lua_tostring(L, -1);
		lua_pop(L, 1);

		lua_getfield(L, 1, "code");
		if (!lua_isstring(L, -1))
		{
			return luaL_error(L, "expected a 'code' string field in the table argument");
		}
		asm_code = lua_tostring(L, -1);
		lua_pop(L, 1);
	}
	else if (n == 2 && lua_isstring(L, 1) && lua_isstring(L, 2))
	{
		arch_str = lua_tostring(L, 1);
		asm_code = lua_tostring(L, 2);
	}
	else
	{
		return luaL_error(L, "expected either a table argument or two string arguments");
	}

	asmjit::Arch arch;
	if (_stricmp(arch_str, "x64") == 0)
	{
		arch = asmjit::Arch::kX64;
	}
	else if (_stricmp(arch_str, "x86") == 0)
	{
		arch = asmjit::Arch::kX86;
	}
	else
	{
		return luaL_error(L, "invalid architecture, expected 'x86' or 'x64'");
	}

	asmjit::Environment env(arch);
	asmjit::CodeHolder code;
	code.init(env);
	asmjit::x86::Assembler assembler(&code);
	asmtk::AsmParser parser(&assembler);
	asmjit::Error err = parser.parse(asm_code);
	if (err != asmjit::Error::kOk)
	{
		return 0;
	}
	asmjit::CodeBuffer& buffer = code._text_section._buffer;

	lua_newtable(L);
	for (size_t i = 0; i < buffer.size(); i++)
	{
		lua_pushinteger(L, buffer[i]);
		lua_rawseti(L, -2, i + 1);
	}
	return 1;
}
