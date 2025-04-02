#include <Windows.h>
#include <TlHelp32.h>
#include <DbgHelp.h>
#include <filesystem>
#include <string>
#include <sstream>

namespace fs = std::filesystem;
class SymbolLoader {
private:
	HANDLE hProcess;
	bool Initialized;
public:
	SymbolLoader() : hProcess(NULL), Initialized(false) {
		hProcess = GetCurrentProcess();
		Initialized = SymInitialize(hProcess, NULL, TRUE);
	}
	~SymbolLoader() {};
	bool LoadSymbols(HMODULE hModule);
};
