#include <Windows.h>
#include <iostream>
#include <format>
#include "ue/Engine.h"
#include "hook/Hook.h"
#include "utils/ExceptionHandler.h"
#include "../shared/Ipc.h"

__declspec(dllexport) LRESULT CALLBACK SplitgateCallBack(int code, WPARAM wparam, LPARAM lparam)
{
	ExceptionHandler::Init();

	if (code < 0)
	{
		return CallNextHookEx(Hook::g_hook, code, wparam, lparam);
	}

	MSG* msg = (MSG*)lparam;

	constexpr UINT WM_SPLITGATE_INIT = WM_APP + 1;
	if (msg->message != WM_SPLITGATE_INIT || g_initialized)
	{
		return CallNextHookEx(Hook::g_hook, code, wparam, lparam);
	}

	Hook::g_hook = reinterpret_cast<HHOOK>(msg->lParam);

	if (!Hook::Init())
	{
		return CallNextHookEx(Hook::g_hook, code, wparam, HCBT_CREATEWND);
	}

	Logger::Log("SUCCESS", "Injected");
	Logger::Log("INFO", std::format("Base Address: [0x{:x}]", (uintptr_t)GetModuleHandleW(0)).c_str());
	Logger::Log("SUCCESS", std::format("Press {} to hide/show the menu", ImGui::VirtualKeyCodeToString(Settings.MENU.ShowHotkey)));

	Ipc::Signal(Ipc::Event::Initialized);
	DiscordRPC::Init();

	return CallNextHookEx(Hook::g_hook, code, wparam, lparam);
}