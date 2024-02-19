#include <Windows.h>
#include <iostream>
#include <format>
#include "ue/Engine.h"
#include "hook/Hook.h"
#include "gui/ZeroGUI.h"
#include "hook/functions/PostRender.h"

__declspec(dllexport) LRESULT CALLBACK SplitgateCallBack(int code, WPARAM wparam, LPARAM lparam) {
	MSG* msg = (MSG*)lparam;
	if (msg->message != HCBT_CREATEWND) return CallNextHookEx(0, code, wparam, lparam);

	if (!Hook::Init()) return CallNextHookEx(Hook::g_hook, code, wparam, HCBT_CREATEWND);

	Hook::OriginalPostRender = reinterpret_cast<decltype(Hook::OriginalPostRender)>(Hook::SetHook(Hook::PostRenderVTable, Hook::PostRenderIndex, &PostRender));

	Logger::Log("SUCCESS", "Injected");
	Logger::Log("INFO", std::format("Base Address: [0x{:x}]", (uintptr_t)GetModuleHandleW(0)).c_str());
	Logger::Log("SUCCESS", std::format("Press {} to hide/show the menu", ZeroGUI::VirtualKeyCodeToString(Settings.MENU.ShowHotkey)));

	Hook::features->rpc->Init(Settings.MISC.DiscordAppID);
	Hook::features->rpc->UpdatePresence();

	return CallNextHookEx(Hook::g_hook, code, wparam, lparam);
}