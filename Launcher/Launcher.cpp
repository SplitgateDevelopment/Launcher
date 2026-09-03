#include <thread>
#include <chrono>
#include "../shared/Ipc.h"
#include "utils/Logger.h"
#include "utils/handles/UniqueHandle.h"
#include "utils/handles/UniqueHook.h"
#include "utils/handles/UniqueLibrary.h"
#include "utils/ProxyConfig.h"
#include "utils/Mitmproxy.h"

int main()
{
	SetConsoleTitleA("Splitgate Launcher");

	Logger logger;
	logger.info("Loading...");

	const auto network = Launcher::ReadNetworkSettings();
	if (network.Proxy == ProxyMode::Mitmproxy)
	{
		if (Launcher::Mitmproxy::Spawn(network.Redirects))
			logger.success("Spawned mitmproxy");
		else
			logger.error("Failed to spawn mitmproxy (is mitmdump on PATH?)");
	}

	Launcher::UniqueHandle initEvent(Ipc::Create(Ipc::Event::Initialized));
	if (!initEvent)
	{
		logger.errorBox(TEXT("CreateEventW"));
		return logger.stop(-1);
	}

	Launcher::UniqueLibrary lib(LoadLibraryA("Internal.dll"));
	if (!lib)
	{
		logger.errorBox(TEXT("LoadLibraryA"));
		return logger.stop(-1);
	}
	logger.success("Loaded target module!");

	HOOKPROC proc = reinterpret_cast<HOOKPROC>(GetProcAddress(lib.get(), "?SplitgateCallBack@@YA_JH_K_J@Z"));
	if (!proc)
	{
		logger.errorBox(TEXT("GetProcAddress"));
		return logger.stop(-1);
	}
	logger.success("Got exported function address!");

	HWND GameWindow = FindWindowA(nullptr, "PortalWars  ");
	if (!GameWindow)
	{
		logger.error("Failed to get game window!");
		logger.errorBox(TEXT("FindWindowA"));

		return logger.stop(-1);
	}
	logger.success("Got game window!");

	DWORD ProcessID = 0, threadId = GetWindowThreadProcessId(GameWindow, &ProcessID);
	if (!threadId)
	{
		logger.error("Failed to get thread id!");
		logger.errorBox(TEXT("GetWindowThreadProcessId"));

		return logger.stop(-1);
	}
	logger.success(std::format("Thread id: {}", threadId));
	logger.success(std::format("Process id: {}", ProcessID));

	Launcher::UniqueHook hook(SetWindowsHookExW(WH_GETMESSAGE, proc, lib.get(), threadId));
	if (!hook)
	{
		logger.errorBox(TEXT("SetWindowsHookExW"));
		return logger.stop(-1);
	}
	logger.success("Placed hook!");

	const UINT initMsg = RegisterWindowMessageW(L"SplitgateInit");
	if (!initMsg)
	{
		logger.errorBox(TEXT("RegisterWindowMessageW"));
		return logger.stop(-1);
	}
	logger.success("Registered window message!");

	constexpr UINT WM_SPLITGATE_INIT = WM_APP + 1;
	if (!PostThreadMessageW(threadId, initMsg, 0, reinterpret_cast<LPARAM>(hook.get())))
	{
		logger.errorBox(TEXT("PostThreadMessageW"));
		return logger.stop(-1);
	}
	logger.success("DLL injected into process!");

	constexpr DWORD TIMEOUT = 15000;
	if (!Ipc::Wait(initEvent.get(), TIMEOUT))
	{
		logger.error("DLL failed to initialize (timed out)!");
		return logger.stop(-1);
	}
	logger.success("DLL initialized successfully!");

	std::this_thread::sleep_for(std::chrono::seconds(2));
	hook.release();

	return 0;
}