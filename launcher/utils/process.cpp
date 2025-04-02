#include "process.h"

namespace utils {
	bool process::initialize(std::uint32_t pid, const wchar_t* name) {
		HANDLE snapshot_handle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (snapshot_handle == INVALID_HANDLE_VALUE || snapshot_handle == nullptr) {
			return false;
		}

		bool survival = false;
		HANDLE hprocess = INVALID_HANDLE_VALUE;
		PROCESSENTRY32 process_entry = { };
		process_entry.dwSize = sizeof(PROCESSENTRY32);
		bool success = Process32First(snapshot_handle, &process_entry);
		while (success) {
			if (pid) {
				if (pid == process_entry.th32ProcessID) {
					this->pid = process_entry.th32ProcessID;
					this->hprocess = OpenProcess(PROCESS_ALL_ACCESS, false, process_entry.th32ProcessID);
					survival = true;
					break;
				}
			}

			if (name) {
				if (_wcsicmp(reinterpret_cast<const wchar_t*>(process_entry.szExeFile), name) == 0) {
					this->pid = process_entry.th32ProcessID;
					this->hprocess = OpenProcess(PROCESS_ALL_ACCESS, false, process_entry.th32ProcessID);
					survival = true;
					break;
				}
			}
			success = Process32Next(snapshot_handle, &process_entry);
		}
		CloseHandle(snapshot_handle);
		return survival;
	}

	process::~process() {
		for (auto address : records) {
			VirtualFreeEx(hprocess, address, 0, MEM_RELEASE);
		}
		CloseHandle(hprocess);
		hprocess = nullptr;
	}

	std::uintmax_t process::model(const wchar_t* name) {
		HMODULE hmodule = nullptr;
		HANDLE snapshot_handle = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);
		if (snapshot_handle == nullptr || snapshot_handle == INVALID_HANDLE_VALUE) {
			throw std::exception("failed to create snapshot");
		}

		MODULEENTRY32 module_entry{ };
		module_entry.dwSize = sizeof(MODULEENTRY32);
		bool success = Module32First(snapshot_handle, &module_entry);
		if (success) {
			do {
				if (_wcsicmp(reinterpret_cast<const wchar_t*>(module_entry.szModule), name) == 0) {
					hmodule = module_entry.hModule;
					break;
				}
			} while (Module32Next(snapshot_handle, &module_entry));
		}

		CloseHandle(snapshot_handle);
		return reinterpret_cast<std::uintmax_t>(hmodule);
	}

	std::uint32_t process::thread_id() {
		HANDLE snapshot_handle = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, pid);
		if (snapshot_handle == INVALID_HANDLE_VALUE)
			throw std::exception("failed to create snapshot");

		THREADENTRY32 thread_entry = { };
		thread_entry.dwSize = sizeof(THREADENTRY32);
		std::uint32_t tid = 0;
		bool success = Thread32First(snapshot_handle, &thread_entry);
		if (success) {
			do {
				if (thread_entry.th32OwnerProcessID == pid) {
					tid = thread_entry.th32ThreadID;
					break;
				}
			} while (Thread32Next(snapshot_handle, &thread_entry));
		}

		CloseHandle(snapshot_handle);
		return tid;
	}

	void* process::allocate(std::size_t size) {
		void* address = VirtualAllocEx(hprocess, nullptr, size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
		records.push_back(address);
		return address;
	}

	bool process::release(void* address) {
		return VirtualFreeEx(hprocess, address, 0, MEM_RELEASE);
	}
}

bool SearchWStringInMemory(HANDLE hProcess, uintptr_t startAddress, size_t regionSize, const std::wstring& targetString)
{
	std::vector<uint8_t> buffer(regionSize);
	SIZE_T bytesRead = 0;

	if (!ReadProcessMemory(hProcess, reinterpret_cast<LPCVOID>(startAddress), buffer.data(), regionSize, &bytesRead))
	{
		return false;
	}

	const uint8_t* targetBytes = reinterpret_cast<const uint8_t*>(targetString.c_str());
	size_t targetSize = targetString.size() * sizeof(wchar_t);

	if (targetSize == 0 || targetSize > bytesRead)
	{
		return false;
	}

	const uint8_t* bufPtr = buffer.data();
	const size_t searchEnd = bytesRead - targetSize;

	if (targetSize >= 4 && searchEnd >= 32)
	{
		uint8_t firstByte = targetBytes[0];
		uint8_t secondByte = targetBytes[1];

		__m128i firstByteVec = _mm_set1_epi8(firstByte);
		__m128i secondByteVec = _mm_set1_epi8(secondByte);

		size_t simdEnd = (searchEnd >= 15) ? searchEnd - 15 : 0;

		for (size_t i = 0; i <= simdEnd; i += 16)
		{
			__m128i data1 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(bufPtr + i));
			__m128i cmp1 = _mm_cmpeq_epi8(data1, firstByteVec);

			__m128i data2 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(bufPtr + i + 1));
			__m128i cmp2 = _mm_cmpeq_epi8(data2, secondByteVec);

			__m128i combined = _mm_and_si128(cmp1, cmp2);
			uint16_t mask = _mm_movemask_epi8(combined);

			if (mask != 0)
			{
				for (int bit = 0; bit < 16; bit++)
				{
					if (mask & (1u << bit))
					{
						size_t pos = i + bit;
						if (pos > searchEnd) break;

						if (memcmp(bufPtr + pos, targetBytes, targetSize) == 0)
						{
							return true;
						}
					}
				}
			}
		}
		for (size_t i = simdEnd + 16; i <= searchEnd; ++i)
		{
			if (memcmp(bufPtr + i, targetBytes, targetSize) == 0)
			{
				return true;
			}
		}
	}
	else
	{
		for (size_t i = 0; i <= searchEnd; ++i)
		{
			if (memcmp(bufPtr + i, targetBytes, targetSize) == 0)
			{
				return true;
			}
		}
	}
	return false;
}

DWORD GetProcessIdByName(const std::wstring& wprocessName)
{
	UniqueHandle snapshot(CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0));
	DWORD processId = 0;
	if (snapshot.get() == INVALID_HANDLE_VALUE)
	{
		return 0;
	}

	PROCESSENTRY32W processEntry = { 0 };
	processEntry.dwSize = sizeof(PROCESSENTRY32W);

	if (Process32FirstW(snapshot.get(), &processEntry))
	{
		do
		{
			if (wprocessName == processEntry.szExeFile)
			{
				processId = processEntry.th32ProcessID;
				break;
			}
		} while (Process32NextW(snapshot.get(), &processEntry));
	}
	return processId;
}

bool GetModuleBaseAddressAndSize(DWORD processId, const std::wstring& moduleName, uintptr_t& baseAddress, size_t& moduleSize)
{
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, processId);
	if (hSnapshot == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	MODULEENTRY32W moduleEntry = { 0 };
	moduleEntry.dwSize = sizeof(MODULEENTRY32W);

	if (Module32FirstW(hSnapshot, &moduleEntry))
	{
		do
		{
			if (moduleName == moduleEntry.szModule)
			{
				baseAddress = reinterpret_cast<uintptr_t>(moduleEntry.modBaseAddr);
				moduleSize = static_cast<size_t>(moduleEntry.modBaseSize);
				CloseHandle(hSnapshot);
				return true;
			}
		} while (Module32NextW(hSnapshot, &moduleEntry));
	}

	CloseHandle(hSnapshot);
	return false;
}
