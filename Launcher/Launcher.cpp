#include <Windows.h>
#include "Logger.h"

int stop(int code) {
	std::cin.get();
	return code;
};

int main() {
	SetConsoleTitleA("Splitgate Launcher");

	HMODULE lib = LoadLibraryA("internal.dll");
	Logger* logger = new Logger();

	if (!lib) {
		logger->error("Failed to load target module!");
		return stop(-1);
	}
	logger->success("Loaded target module!");

	HOOKPROC proc = reinterpret_cast<HOOKPROC>(GetProcAddress(lib, "?SplitgateCallBack@@YA_JH_K_J@Z"));
	if (!proc) {
		logger->error("Failed to get exported function address!");
		return stop(-1);
	}
	logger->success("Got exported function address!");

	HWND GameWindow = FindWindowA(0, "PortalWars  ");
	if (!GameWindow) {
		logger->error("Failed to get game window!");
		return stop(-1);
	}
	logger->success("Got game window!");

	DWORD ProcessID = 0, ThreadID = GetWindowThreadProcessId(GameWindow, &ProcessID);
	if (!ThreadID) {
		logger->error("Failed to get thread id!");
		return stop(-1);
	}
	logger->success(std::format("Got thread id: {}", ThreadID));
	logger->success(std::format("Got process id: {}", ProcessID));

	HHOOK hook = SetWindowsHookExW(WH_GETMESSAGE, proc, lib, ThreadID);
	if (!hook) {
		logger->error("Failed to place hook");
		return stop(-1);
	};
	logger->success("Placed hook!");

	PostThreadMessageW(ThreadID, HCBT_CREATEWND, 0, reinterpret_cast<LPARAM>(hook));

	logger->success("DLL injected into process!");

	while (FindWindowA(0, "PortalWars  ")) {
		std::cin.ignore(1);
		std::cin.get();
	}

	logger->info("Game window closed");
	return stop(0);
}