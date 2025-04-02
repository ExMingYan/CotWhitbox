#include "DLLInjector.h"
#include <TlHelp32.h>
#include <iostream>
#include <string>

bool DLLInjector::Initialize(DWORD processID) {
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processID);
	if (!hProcess) {
		std::cerr << "Failed to open process with ID " << processID << ". Error: " << GetLastError() << std::endl;
		return false;
	}
	return true;
}

bool DLLInjector::InjectDLL(const std::string& dllPath)
{
	LPVOID pDllPath = VirtualAllocEx(hProcess, nullptr, dllPath.size() + 1, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	if (!pDllPath)
	{
		std::cerr << "Failed to allocate memory in target process. Error: " << GetLastError() << std::endl;
		return false;
	}

	if (!WriteProcessMemory(hProcess, pDllPath, dllPath.c_str(), dllPath.size() + 1, nullptr))
	{
		std::cerr << "Failed to write DLL path to target process memory. Error: " << GetLastError() << std::endl;
		VirtualFreeEx(hProcess, pDllPath, 0, MEM_RELEASE);
		return false;
	}

	HMODULE hmodule = GetModuleHandleW(L"kernel32.dll");
	if (hmodule == NULL || hmodule == INVALID_HANDLE_VALUE)
	{
		std::cerr << "Failed to get handle of kernel32.dll. Error: " << GetLastError() << std::endl;
		VirtualFreeEx(hProcess, pDllPath, 0, MEM_RELEASE);
		return false;
	}

	LPTHREAD_START_ROUTINE pLoadLibrary = (LPTHREAD_START_ROUTINE)GetProcAddress(hmodule, "LoadLibraryA");
	if (!pLoadLibrary)
	{
		std::cerr << "Failed to get address of LoadLibraryA. Error: " << GetLastError() << std::endl;
		VirtualFreeEx(hProcess, pDllPath, 0, MEM_RELEASE);
		return false;
	}

	HANDLE hThread = CreateRemoteThread(hProcess, nullptr, 0, pLoadLibrary, pDllPath, 0, nullptr);
	if (!hThread)
	{
		std::cerr << "Failed to create remote thread. Error: " << GetLastError() << std::endl;
		VirtualFreeEx(hProcess, pDllPath, 0, MEM_RELEASE);
		return false;
	}
	WaitForSingleObject(hThread, INFINITE);
	VirtualFreeEx(hProcess, pDllPath, 0, MEM_RELEASE);
	CloseHandle(hThread);
	return true;
}

bool DLLInjector::EnjectDLL(const std::string& dllName)
{
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, GetProcessId(hProcess));
	if (hSnap == INVALID_HANDLE_VALUE)
	{
		std::cerr << "Failed to create module snapshot. Error: " << GetLastError() << std::endl;
		return false;
	}

	MODULEENTRY32 me32;
	me32.dwSize = sizeof(MODULEENTRY32);
	HMODULE hModule = nullptr;

	if (Module32First(hSnap, &me32))
	{
		do
		{
			if (WcharToString(me32.szModule) == dllName)
			{
				hModule = me32.hModule;
				break;
			}
		} while (Module32Next(hSnap, &me32));
	}

	CloseHandle(hSnap);

	if (hModule == nullptr)
	{
		std::cerr << "DLL not found in target process." << std::endl;
		return false;
	}

	HMODULE hKernel32 = GetModuleHandleW(L"kernel32.dll");
	if (hKernel32 == NULL)
	{
		std::cerr << "Failed to get handle of kernel32.dll. Error: " << GetLastError() << std::endl;
		return false;
	}

	LPTHREAD_START_ROUTINE pFreeLibrary = (LPTHREAD_START_ROUTINE)GetProcAddress(hKernel32, "FreeLibrary");
	if (!pFreeLibrary)
	{
		std::cerr << "Failed to get address of FreeLibrary. Error: " << GetLastError() << std::endl;
		return false;
	}

	HANDLE hThread = CreateRemoteThread(hProcess, nullptr, 0, pFreeLibrary, hModule, 0, nullptr);
	if (!hThread)
	{
		std::cerr << "Failed to create remote thread for FreeLibrary. Error: " << GetLastError() << std::endl;
		return false;
	}

	WaitForSingleObject(hThread, INFINITE);
	CloseHandle(hThread);
	return true;
}

std::string WcharToString(const WCHAR* wstr, UINT codePage)
{
	if (wstr == nullptr)
	{
		return "";
	}

	int requiredSize = WideCharToMultiByte(codePage, 0, wstr, -1, nullptr, 0, nullptr, nullptr);
	if (requiredSize == 0)
	{
		return "";
	}

	std::string str(requiredSize - 1, 0);

	int result = WideCharToMultiByte(codePage, 0, wstr, -1, &str[0], requiredSize, nullptr, nullptr);
	if (result == 0)
	{
		return "";
	}

	return str;
}