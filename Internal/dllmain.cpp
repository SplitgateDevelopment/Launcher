// dllmain.cpp : Definisce il punto di ingresso per l'applicazione DLL.
#include <Windows.h>
#include <iostream>
#include <format>
#include "cheat/UnrealEngine/Engine.h"
#include "cheat/Hook.h"
#include "menu/Menu.h"

Hook* hook = new Hook();
Menu* menu = new Menu();

void PostRender(UGameViewportClient* UGameViewportClient, Canvas* canvas)
{
	do {
		ZeroGUI::SetupCanvas(canvas);
		menu->Tick();
		menu->DrawWatermark(canvas);

		UWorld* World = *(UWorld**)(WRLD);
		if (!World) break;

		UGameInstance* OwningGameInstance = World->OwningGameInstance;
		if (!OwningGameInstance) break;

		TArray<UPlayer*> LocalPlayers = OwningGameInstance->LocalPlayers;

		UPlayer* LocalPlayer = LocalPlayers[0];
		if (!LocalPlayer) break;

		APlayerController* PlayerController = LocalPlayer->PlayerController;
		if (!PlayerController) break;

		/*if(Settings::UnHook) {
			hook->swapRender(LocalPlayer->ViewportClient->VFTable, OPostRender);
			OPostRender(UGameViewportClient, canvas);
			UnhookWindowsHookEx(hook->g_hook);
		}*/

		hook->features->handle(PlayerController);
	} while (false);

	return hook->OriginalPostRender(UGameViewportClient, canvas);
}

void ProcessEvent(UObject* pe, UObject* fn, void* params) {
	return hook->OriginalProcessEvent(pe, fn, params);
};

__declspec(dllexport) LRESULT CALLBACK SplitgateCallBack(int code, WPARAM wparam, LPARAM lparam) {
	MSG* msg = (MSG*)lparam;
	if (msg->message != HCBT_CREATEWND) return CallNextHookEx(0, code, wparam, lparam);

	if (!hook->Init()) return CallNextHookEx(hook->g_hook, code, wparam, HCBT_CREATEWND);

	hook->OriginalPostRender = reinterpret_cast<decltype(hook->OriginalPostRender)>(hook->setHook(hook->PostRenderVTable, hook->PostRenderIndex, &PostRender));
	hook->OriginalProcessEvent = reinterpret_cast<decltype(hook->OriginalProcessEvent)>(hook->setHook(hook->ProcessEventVTable, hook->ProcessEventIndex, &ProcessEvent));

	hook->logger->log("SUCCESS", "Injected");
	hook->logger->log("INFO", std::format("Base Address: [0x{:x}]", (uintptr_t)GetModuleHandleW(0)).c_str());
	hook->logger->log("SUCCESS", "Press Ins to hide/show the menu");

	return CallNextHookEx(hook->g_hook, code, wparam, lparam);
}