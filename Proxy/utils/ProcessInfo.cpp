#include "ProcessInfo.h"
#include "searcher.h"
#include <string>
#include <vector>
#include <TlHelp32.h>
#include <Psapi.h>

HWND GetProcessMainWindow(DWORD processId, std::string UnrealWindow);

const char* GetProcessFolderPath()
{
	static char path[MAX_PATH] = { 0 };
	if (path[0] == 0)
	{
		char fullPath[MAX_PATH] = { 0 };
		GetModuleFileNameA(NULL, fullPath, MAX_PATH);
		char* lastBackslash = strrchr(fullPath, '\\');
		if (lastBackslash)
		{
			size_t length = static_cast<size_t>(lastBackslash - fullPath);
			strncpy_s(path, fullPath, length);
			path[length] = '\0';
		}
	}
	return path;
}

const std::wstring GetProcessName()
{
	static std::wstring processName;
	if (processName.empty())
	{
		wchar_t fullPath[MAX_PATH] = { 0 };
		GetModuleFileNameW(NULL, fullPath, MAX_PATH);
		wchar_t* lastBackslash = wcsrchr(fullPath, L'\\');
		if (lastBackslash)
		{
			processName = lastBackslash + 1;
		}
		else
		{
			processName = fullPath;
		}
	}
	return processName;
}

DWORD GetProcessID()
{
	return GetCurrentProcessId();
}

HANDLE GetProcessHandle()
{
	return GetCurrentProcess();
}

bool WindowLoaded(std::string WindowClass)
{
	HWND hwnd = GetProcessMainWindow(GetProcessID(), WindowClass);
	if (hwnd != NULL)
	{
		WNDPROC wndProc = (WNDPROC)GetWindowLongPtrA(hwnd, GWLP_WNDPROC);
		if (wndProc != NULL)
		{
			return true;
		}
	}
	return false;
}

bool CheckGameVersion(const std::wstring versionSymbol)
{
	HANDLE hProcess = GetProcessHandle();
	if (hProcess == NULL)
		return false;
	uintptr_t moduleAddress;
	size_t moduleSize;
	bool success = GetModuleBaseAddressAndSize(GetProcessName(), moduleAddress, moduleSize);
	if (!success)
		return false;
	return SearchWStringInMemory(hProcess, moduleAddress, moduleSize, versionSymbol);
}

bool GetModuleBaseAddressAndSize(const std::wstring& moduleName, uintptr_t& baseAddress, size_t& moduleSize)
{
	HMODULE hModule = GetModuleHandleW(moduleName.c_str());
	if (hModule == NULL)
		return false;

	MODULEINFO moduleInfo;
	if (!GetModuleInformation(GetCurrentProcess(), hModule, &moduleInfo, sizeof(moduleInfo)))
		return false;

	baseAddress = reinterpret_cast<uintptr_t>(moduleInfo.lpBaseOfDll);
	moduleSize = moduleInfo.SizeOfImage;
	return true;
}

bool isAddressAccessAble(intptr_t address)
{
	if ((void*)address == nullptr)
	{
		return false;
	}
	MEMORY_BASIC_INFORMATION mbi;
	if (VirtualQuery((LPCVOID)address, &mbi, sizeof(mbi)) == 0)
	{
		return false;
	}
	if (!(mbi.State & MEM_COMMIT))
	{
		return false;
	}
	if (mbi.Protect & (PAGE_NOACCESS | PAGE_GUARD))
	{
		return false;
	}
	return true;
}

BOOL CALLBACK EnumWindowsCallback(HWND hwnd, LPARAM lParam)
{
	HandleData& data = *(HandleData*)lParam;
	DWORD processId = 0;
	GetWindowThreadProcessId(hwnd, &processId);
	if (processId != data.processId || !IsWindowVisible(hwnd))
	{
		return TRUE;
	}
	if (GetWindow(hwnd, GW_OWNER) != NULL)
	{
		return TRUE;
	}
	if (!data.windowClass.empty())
	{
		char className[256];
		GetClassNameA(hwnd, className, sizeof(className));
		if (data.windowClass != className)
		{
			return TRUE;
		}
	}
	data.windowHandle = hwnd;
	return FALSE;
}

HWND GetProcessMainWindow(DWORD processId, std::string WindowClass)
{
	HandleData data;
	data.processId = processId;
	data.windowHandle = NULL;
	data.windowClass = WindowClass;
	EnumWindows(EnumWindowsCallback, (LPARAM)&data);
	return data.windowHandle;
}
