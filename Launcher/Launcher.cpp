#include "utils/Logger.h"
#include <thread>
#include <chrono>

int main() {
	SetConsoleTitleA("Splitgate Launcher");
	Logger* logger = new Logger();

	HMODULE lib = LoadLibraryA("internal.dll");
	if (!lib) {
		logger->error("Failed to load target module!");
		logger->errorBox(TEXT("LoadLibraryA"));
		return logger->stop(-1);
	}
	logger->success("Loaded target module!");

	HOOKPROC proc = reinterpret_cast<HOOKPROC>(GetProcAddress(lib, "?SplitgateCallBack@@YA_JH_K_J@Z"));
	if (!proc) {
		logger->error("Failed to get exported function address!");
		logger->errorBox(TEXT("HOOKPROC"));
		return logger->stop(-1);
	}
	logger->success("Got exported function address!");

	HWND GameWindow = FindWindowA(0, "PortalWars  ");
	if (!GameWindow) {
		logger->error("Failed to get game window!");
		return logger->stop(-1);
	}
	logger->success("Got game window!");

	DWORD ProcessID = 0, ThreadID = GetWindowThreadProcessId(GameWindow, &ProcessID);
	if (!ThreadID) {
		logger->error("Failed to get thread id!");
		logger->errorBox(TEXT("GetWindowThreadProcessId"));
		return logger->stop(-1);
	}
	logger->success(std::format("Thread id: {}", ThreadID));
	logger->success(std::format("Process id: {}", ProcessID));

	HHOOK hook = SetWindowsHookExW(WH_GETMESSAGE, proc, lib, ThreadID);
	if (!hook) {
		logger->error("Failed to place hook");
		logger->errorBox(TEXT("SetWindowsHookExW"));
		return logger->stop(-1);
	};
	logger->success("Placed hook!");

	if (!PostThreadMessageW(ThreadID, HCBT_CREATEWND, 0, reinterpret_cast<LPARAM>(hook))) {
		logger->error("Failed to post thread message!");
		logger->errorBox(TEXT("PostThreadMessageW"));
		return logger->stop(-1);
	}
	logger->success("DLL injected into process!");

	std::this_thread::sleep_for(std::chrono::seconds(2));
	return 0;
}