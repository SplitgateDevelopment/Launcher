#include "utils/Logger.h"
#include <thread>
#include <chrono>

int main()
{
	SetConsoleTitleA("Splitgate Launcher");

	Logger logger;
	logger.info("Loading...");

	HMODULE lib = LoadLibraryA("internal.dll");
	if (!lib)
	{
		logger.error("Failed to load target module!");
		logger.errorBox(TEXT("LoadLibraryA"));

		FreeLibrary(lib);
		return logger.stop(-1);
	}
	logger.success("Loaded target module!");

	HOOKPROC proc = reinterpret_cast<HOOKPROC>(GetProcAddress(lib, "?SplitgateCallBack@@YA_JH_K_J@Z"));
	if (!proc)
	{
		logger.error("Failed to get exported function address!");
		logger.errorBox(TEXT("HOOKPROC"));

		FreeLibrary(lib);
		return logger.stop(-1);
	}
	logger.success("Got exported function address!");

	HWND GameWindow = FindWindowA(nullptr, "PortalWars  ");
	if (!GameWindow)
	{
		logger.error("Failed to get game window!");
		logger.errorBox(TEXT("FindWindowA"));

		FreeLibrary(lib);
		return logger.stop(-1);
	}
	logger.success("Got game window!");

	DWORD ProcessID = 0, ThreadID = GetWindowThreadProcessId(GameWindow, &ProcessID);
	if (!ThreadID)
	{
		logger.error("Failed to get thread id!");
		logger.errorBox(TEXT("GetWindowThreadProcessId"));

		FreeLibrary(lib);
		return logger.stop(-1);
	}
	logger.success(std::format("Thread id: {}", ThreadID));
	logger.success(std::format("Process id: {}", ProcessID));

	HHOOK hook = SetWindowsHookExW(WH_GETMESSAGE, proc, lib, ThreadID);
	if (!hook)
	{
		logger.error("Failed to place hook");
		logger.errorBox(TEXT("SetWindowsHookExW"));

		UnhookWindowsHookEx(hook);
		FreeLibrary(lib);

		return logger.stop(-1);
	};
	logger.success("Placed hook!");

	constexpr UINT WM_SPLITGATE_INIT = WM_APP + 1;
	if (!PostThreadMessageW(ThreadID, WM_SPLITGATE_INIT, 0, reinterpret_cast<LPARAM>(hook)))
	{
		logger.error("Failed to post thread message!");
		logger.errorBox(TEXT("PostThreadMessageW"));

		UnhookWindowsHookEx(hook);
		FreeLibrary(lib);

		return logger.stop(-1);
	}
	logger.success("DLL injected into process!");

	std::this_thread::sleep_for(std::chrono::seconds(2));
	return 0;
}