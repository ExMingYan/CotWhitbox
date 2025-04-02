#include <iostream>
#include <windows.h>
#include "utils/loader.h"
#include "utils/process.h"
#include "hitbox.h"
#pragma comment(linker, "/subsystem:windows /entry:mainCRTStartup")

#ifndef _WIN64
#error errors requires x64 environment
#endif

static const std::wstring processName = L"CotW-Win64-Shipping.exe";

int main()
{
	try
	{
		HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, GetProcessIdByName(processName));
		if (hProcess == INVALID_HANDLE_VALUE || hProcess == 0)
			throw std::exception("process invalid");
		uintptr_t moduleAddress;
		size_t moduleSize;
		bool success = GetModuleBaseAddressAndSize(GetProcessIdByName(processName), processName, moduleAddress, moduleSize);
		if (!success || moduleAddress == 0 || moduleSize == 0)
			throw std::exception("module invalid");
		bool versionCheck = SearchWStringInMemory(hProcess, moduleAddress, moduleSize, L"108052");
		if (!versionCheck)
			throw std::exception("version invalid");
		utils::loader load;
		load.shadow(processName.c_str(), hitbox, sizeof(hitbox));
	}
	catch (std::exception& e)
	{
		MessageBoxA(0, e.what(), "warning", MB_OK);
	}
	return 0;
}
