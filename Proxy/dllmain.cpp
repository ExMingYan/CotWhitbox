#include "LuaExec/LuaExec.h"
#include "Proxy/Proxy.h"
#include "utils/Logger.hpp"

#include <exception>
#include <windows.h>

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	{
		loggers::initialize();
		InitializeProxy();
		try
		{
			ExecuteLuaFile();
		}
		catch (std::exception& e)
		{
			loggers::information(__FUNCTION__, __LINE__, "Failed to execute Lua File. %s", e.what());
		}
		break;
	}
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		FinalizeProxy();
		break;
	}
	return TRUE;
}
