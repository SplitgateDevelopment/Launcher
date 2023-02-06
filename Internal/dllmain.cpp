// dllmain.cpp : Definisce il punto di ingresso per l'applicazione DLL.

#include <Windows.h>
#include <iostream>
#include <format>
#include "Engine.h"
#include "Hook.h"

Hook* hook = new Hook();

void PostRender(UGameViewportClient* UGameViewportClient, Canvas* canvas)
{
	do {
		UWorld* World = *(UWorld**)(WRLD);
		if (!World) break;

		UGameInstance* OwningGameInstance = World->OwningGameInstance;
		if (!OwningGameInstance) break;

		TArray<UPlayer*> LocalPlayers = OwningGameInstance->LocalPlayers;

		UPlayer* LocalPlayer = LocalPlayers[0];
		if (!LocalPlayer) break;

		APlayerController* PlayerController = LocalPlayer->PlayerController;
		if (!PlayerController) break;

		PlayerController->SetName(L"SplitgateDevelopment");

		if (GetAsyncKeyState(VK_INSERT) & 1) {
			hook->logger->log("INFO", "Loading map");
			PlayerController->SwitchLevel(L"Simulation_Alpha?Name=SplitgateDevelopment&gameMode=SHOTSNIP");
		};

		if (GetAsyncKeyState(VK_DELETE) && 1) {
			hook->swapRender(LocalPlayer->ViewportClient->VFTable, OPostRender);
			UnhookWindowsHookEx(hook->g_hook);
		}

		if (GetAsyncKeyState(VK_RCONTROL) & 1) hook->logger->log("INFO", std::format("Changed fov to {}", hook->features->updateFov()));

		if (GetAsyncKeyState(VK_RSHIFT) & 1) {
			hook->features->GodMode = !hook->features->GodMode;
			hook->logger->log("INFO", std::format("Changed hook->features->GodMode to {}", hook->features->GodMode));
		};

		if (GetAsyncKeyState(VK_F1) & 1) {
			hook->features->IncreasedSpeed = !hook->features->IncreasedSpeed;
			hook->logger->log("INFO", std::format("Changed hook->features->IncreasedSpeed to {}", hook->features->IncreasedSpeed));
		}

		if (GetAsyncKeyState(VK_F2) & 1) {
			hook->features->SuperiorMelee = !hook->features->SuperiorMelee;
			hook->logger->log("INFO", std::format("Changed hook->features->SuperiorMelee to {}", hook->features->SuperiorMelee));
		}

		if (GetAsyncKeyState(VK_F3) & 1) {
			hook->features->InfiniteJetpack = !hook->features->InfiniteJetpack;
			hook->logger->log("INFO", std::format("Changed hook->features->InfiniteJetpack to {}", hook->features->InfiniteJetpack));
		}

		if (GetAsyncKeyState(VK_F4) & 1) {
			hook->features->DisableOutOfBounds = !hook->features->DisableOutOfBounds;
			hook->logger->log("INFO", std::format("Changed hook->features->DisableOutOfBounds to {}", hook->features->DisableOutOfBounds));
		}

		if (GetAsyncKeyState(VK_F5) & 1) {
			hook->features->ExtendedSprays = !hook->features->ExtendedSprays;
			hook->logger->log("INFO", std::format("Changed hook->features->ExtendedSprays to {}", hook->features->ExtendedSprays));
		}

		hook->features->handle(PlayerController);
	} while (false);

	OPostRender(UGameViewportClient, canvas);
}


__declspec(dllexport) LRESULT CALLBACK SplitgateCallBack(int code, WPARAM wparam, LPARAM lparam) {
    MSG* msg = (MSG*)lparam;
    if (msg->message != HCBT_CREATEWND) return CallNextHookEx(0, code, wparam, lparam);

    if (!hook->Init()) return CallNextHookEx(hook->g_hook, code, wparam, HCBT_CREATEWND);

	OPostRender = reinterpret_cast<decltype(OPostRender)>(hook->VTABLE[100]);
	hook->swapRender(hook->VTABLE, PostRender);

	hook->logger->log("SUCCESS", "Injected");
	hook->logger->log("INFO", std::format("Base Address: 0x{:x}\n", (uintptr_t)GetModuleHandleW(0)).c_str());

    return CallNextHookEx(hook->g_hook, code, wparam, lparam);
}