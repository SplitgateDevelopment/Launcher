#include <Windows.h>
#include <iostream>
#include <format>
#include "ue/Engine.h"
#include "hook/Hook.h"
#include "hook/functions/PostRender.h"
#include "utils/ExceptionHandler.h"

__declspec(dllexport) LRESULT CALLBACK SplitgateCallBack(int code, WPARAM wparam, LPARAM lparam) {
	ExceptionHandler::Init();

    MSG* msg = (MSG*)lparam;
	if (msg->message != HCBT_CREATEWND) return CallNextHookEx(Hook::g_hook, code, wparam, lparam);

	if (!Hook::Init()) return CallNextHookEx(Hook::g_hook, code, wparam, HCBT_CREATEWND);

	Hook::PostRender::Original = reinterpret_cast<decltype(Hook::PostRender::Original)>(Hook::SetHook(Hook::PostRender::VTable, Hook::PostRender::Index, &PostRender));

	Logger::Log("SUCCESS", "Injected");
	Logger::Log("INFO", std::format("Base Address: [0x{:x}]", (uintptr_t)GetModuleHandleW(0)).c_str());
	Logger::Log("SUCCESS", std::format("Press {} to hide/show the menu", ImGui::VirtualKeyCodeToString(Settings.MENU.ShowHotkey)));

	DiscordRPC::Init(Settings.MISC.DiscordAppID);

	return CallNextHookEx(Hook::g_hook, code, wparam, lparam);
}