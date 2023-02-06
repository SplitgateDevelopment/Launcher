// dllmain.cpp : Definisce il punto di ingresso per l'applicazione DLL.

#include <Windows.h>
#include <iostream>
#include <format>
#include "Engine.h"
#include "logger.h"

HHOOK g_hook;
Logger* logger = new Logger({
    FALSE,
});

void swapRender(void** ViewPortClientVTable, void (*NPostRender)(UGameViewportClient* UGameViewportClient, Canvas* canvas)) {

    DWORD protecc;
    VirtualProtect(&ViewPortClientVTable[100], 8, PAGE_EXECUTE_READWRITE, &protecc);
    ViewPortClientVTable[100] = NPostRender;
    VirtualProtect(&ViewPortClientVTable[100], 8, protecc, 0);

    logger->log("INFO", "Swapped PostRender functions");
}

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
			logger->log("INFO", "Loading map");
			PlayerController->SwitchLevel(L"Simulation_Alpha?Name=SplitgateDevelopment&gameMode=SHOTSNIP");
		};

        if (GetAsyncKeyState(VK_DELETE) && 1) {
            swapRender(LocalPlayer->ViewportClient->VFTable, OPostRender);
            UnhookWindowsHookEx(g_hook);
        }

        if (GetAsyncKeyState(VK_RCONTROL) & 1) {
            float newFov = 140;
            logger->log("INFO", "Changing fov");

            PlayerController->FOV(newFov);
            logger->log("INFO", std::format("Changed fov to {}", newFov));
        };
	} while (false);

	OPostRender(UGameViewportClient, canvas);
}

__declspec(dllexport) LRESULT CALLBACK SplitgateCallBack(int code, WPARAM wparam, LPARAM lparam) {
    MSG* msg = (MSG*)lparam;
    if (msg->message != HCBT_CREATEWND) return CallNextHookEx(0, code, wparam, lparam);

    if (!EngineInit()) {
        logger->log("ERROR", "No engine init");
        return CallNextHookEx(g_hook, code, wparam, HCBT_CREATEWND);
    };

    UWorld* World = *(UWorld**)(WRLD);
    if (!World) {
        logger->log("ERROR", "No World");
        return CallNextHookEx(g_hook, code, wparam, HCBT_CREATEWND);
    };

    UGameInstance* OwningGameInstance = World->OwningGameInstance;
    if (!OwningGameInstance) {
        logger->log("ERROR", "No owning game instance");
        return CallNextHookEx(g_hook, code, wparam, HCBT_CREATEWND);
    };

    TArray<UPlayer*>LocalPlayers = OwningGameInstance->LocalPlayers;

    UPlayer* LocalPlayer = LocalPlayers[0];
    if (!LocalPlayer) {
        logger->log("ERROR", "No LocalPlayer");
        return CallNextHookEx(g_hook, code, wparam, HCBT_CREATEWND);
    };

    UGameViewportClient* ViewPortClient = LocalPlayer->ViewportClient;
    if (!ViewPortClient) {
        logger->log("ERROR", "No UGameViewportClient");
        return CallNextHookEx(g_hook, code, wparam, HCBT_CREATEWND);
    };

    void** ViewPortClientVTable = ViewPortClient->VFTable;
    if (!ViewPortClientVTable) {
        logger->log("ERROR", "No ViewPortClientVTable");
        return CallNextHookEx(g_hook, code, wparam, HCBT_CREATEWND);
    };

    OPostRender = reinterpret_cast<decltype(OPostRender)>(ViewPortClientVTable[100]);
    swapRender(ViewPortClientVTable, PostRender);

    logger->log("SUCCESS", "Injected");
    logger->log("INFO", std::format("Base Address: 0x{:x}\n", (uintptr_t)GetModuleHandleW(0)).c_str());

    return CallNextHookEx(g_hook, code, wparam, lparam);
}