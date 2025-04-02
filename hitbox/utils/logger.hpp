#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <iostream>
#include <windows.h>

#ifdef _DEBUG
#define logger(message, ...) loggers::information(__FUNCTION__, __LINE__, message, __VA_ARGS__)
#else
#define logger(message, ...)
#endif

class loggers {
public:
	static void initialize() {
#ifdef _DEBUG
		if (isexists()) {
			return;
		}
		AllocConsole();
		freopen("CONOUT$", "w+t", stdout);
		freopen("CONOUT$", "w+t", stderr);
		freopen("CONIN$", "r+t", stdin);
#endif
		return;
	}

	static bool isexists() {
		HWND consoleWindow = GetConsoleWindow();
		return consoleWindow != nullptr;
	}

	static void finalize() {
#ifdef _DEBUG
		if (!isexists()) {
			return;
		}
		fclose(stdout);
		fclose(stderr);
		fclose(stdin);
		FreeConsole();
#endif
	}

	template<typename... args>
	static void information(const char* func, int line, const char* message, args&&... params) {
		if (!isexists()) {
			return;
		}
		char buffer[512]{};
		sprintf(buffer, message, params...);
		printf("%s | %d | %s\n", func, line, buffer);
		return;
	}
};

#endif // LOGGER_HPP
