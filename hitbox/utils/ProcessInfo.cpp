#include "ProcessInfo.h"
#include "searcher.h"
#include <string>
#include <vector>
#include <TlHelp32.h>
#include <Psapi.h>

HWND GetProcessMainWindow(DWORD processId, std::string UnrealWindow);

const char* GetModuleFolderPath(const std::string& moduleName)
{
	static char path[MAX_PATH] = { 0 };
	if (path[0] == 0)
	{
		char fullPath[MAX_PATH] = { 0 };
		if (moduleName.empty())
		{
			GetModuleFileNameA(NULL, fullPath, MAX_PATH);
		}
		else
		{
			HMODULE hModule = GetModuleHandleA(moduleName.c_str());
			if (hModule == NULL)
			{
				return nullptr;
			}
			GetModuleFileNameA(hModule, fullPath, MAX_PATH);
		}
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

const std::string GetProcessName()
{
	static std::string processName;
	if (processName.empty())
	{
		char fullPath[MAX_PATH] = { 0 };
		GetModuleFileNameA(NULL, fullPath, MAX_PATH);
		char* lastBackslash = strrchr(fullPath, '\\');
		if (lastBackslash)
		{
			processName = std::string(lastBackslash + 1);
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

bool ModuleExists(const std::string& moduleName)
{
	HMODULE hModule = GetModuleHandleA(moduleName.c_str());
	return hModule != NULL;
}

bool GetModuleBaseAddressAndSize(const std::string& moduleName, uintptr_t& baseAddress, size_t& moduleSize)
{
	HMODULE hModule = GetModuleHandleA(moduleName.c_str());
	if (hModule == NULL)
		return false;

	MODULEINFO moduleInfo;
	if (!GetModuleInformation(GetCurrentProcess(), hModule, &moduleInfo, sizeof(moduleInfo)))
		return false;

	baseAddress = reinterpret_cast<uintptr_t>(moduleInfo.lpBaseOfDll);
	moduleSize = moduleInfo.SizeOfImage;
	return true;
}

bool GetModuleSectionAddressAndSize(const std::string& moduleName, const std::string& sectionName, uintptr_t& sectionAddress, size_t& sectionSize)
{
	HMODULE hModule = GetModuleHandleA(moduleName.c_str());
	if (hModule == NULL)
		return false;
	unsigned char* baseAddress = reinterpret_cast<unsigned char*>(hModule);
	PIMAGE_DOS_HEADER dosHeader = reinterpret_cast<PIMAGE_DOS_HEADER>(baseAddress);
	PIMAGE_NT_HEADERS ntHeaders = reinterpret_cast<PIMAGE_NT_HEADERS>(baseAddress + dosHeader->e_lfanew);
	PIMAGE_SECTION_HEADER sectionHeader = IMAGE_FIRST_SECTION(ntHeaders);
	for (WORD i = 0; i < ntHeaders->FileHeader.NumberOfSections; ++i)
	{
		std::string currentSectionName(reinterpret_cast<char*>(sectionHeader->Name), strnlen_s(reinterpret_cast<char*>(sectionHeader->Name), IMAGE_SIZEOF_SHORT_NAME));
		if (currentSectionName == sectionName)
		{
			sectionAddress = reinterpret_cast<uintptr_t>(baseAddress + sectionHeader->VirtualAddress);
			sectionSize = static_cast<size_t>(sectionHeader->Misc.VirtualSize);
			return true;
		}
		++sectionHeader;
	}
	return false;
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

DWORD GetAddressProtect(intptr_t address)
{
	if ((void*)address == nullptr)
		return 0;
	MEMORY_BASIC_INFORMATION mbi;
	SIZE_T result = VirtualQuery((LPCVOID)address, &mbi, sizeof(mbi));
	if (result == 0)
	{
		return 0;
	}
	else if (!(mbi.State & MEM_COMMIT))
	{
		return 0;
	}
	return mbi.Protect;
}

bool isAddressExecutable(intptr_t address)
{
	DWORD protect = GetAddressProtect(address);
	if (protect == 0)
	{
		return false;
	}
	const DWORD executeMask = PAGE_EXECUTE | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY;
	return (protect & executeMask) != 0;
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
