#pragma once
#include <string>
#include <windows.h>
#include <winnt.h>
#include <TlHelp32.h>
#include <immintrin.h>

struct HandleData
{
	DWORD processId;
	HWND windowHandle;
	std::string windowClass;
};

struct RegistersInfo
{
	DWORD64 Rax, Rbx, Rcx, Rdx, Rsi, Rdi, Rsp, Rbp;
	DWORD64 R8, R9, R10, R11, R12, R13, R14, R15;
	DWORD64 Rip;
	DWORD EFlags;
	DWORD SegCs, SegDs, SegEs, SegFs, SegGs, SegSs;
	M128A XmmRegisters[16];
};

const char* GetProcessFolderPath();
DWORD GetProcessID();
const std::wstring GetProcessName();
HANDLE GetProcessHandle();
bool WindowLoaded(std::string WindowClass);
bool CheckGameVersion(const std::wstring versionSymbol);
bool GetModuleBaseAddressAndSize(const std::wstring& moduleName, uintptr_t& baseAddress, size_t& moduleSize);
bool isAddressAccessAble(intptr_t address);
