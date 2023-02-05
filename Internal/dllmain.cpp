// dllmain.cpp : Definisce il punto di ingresso per l'applicazione DLL.

#include <Windows.h>
#include <iostream>
#include <format>
#include "Engine.h"
#include "logger.h"

Logger* logger = new Logger({
    FALSE,
});

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
		
		if (GetAsyncKeyState(VK_INSERT) & 1) {
			logger->log("INFO", "Loading map");
			PlayerController->SwitchLevel(L"Simulation_Alpha?gameMode=SHOTSNIP");
		};

        if (GetAsyncKeyState(VK_DELETE) & 1) {
            float newFov = 120;
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
        return CallNextHookEx(NULL, code, wparam, HCBT_CREATEWND);
    };

    UWorld* World = *(UWorld**)(WRLD);
    if (!World) {
        logger->log("ERROR", "No World");
        return CallNextHookEx(NULL, code, wparam, HCBT_CREATEWND);
    };

    UGameInstance* OwningGameInstance = World->OwningGameInstance;
    if (!OwningGameInstance) {
        logger->log("ERROR", "No owning game instance");
        return CallNextHookEx(NULL, code, wparam, HCBT_CREATEWND);
    };

    TArray<UPlayer*>LocalPlayers = OwningGameInstance->LocalPlayers;

    UPlayer* LocalPlayer = LocalPlayers[0];
    if (!LocalPlayer) {
        logger->log("ERROR", "No LocalPlayer");
        return CallNextHookEx(NULL, code, wparam, HCBT_CREATEWND);
    };

    UGameViewportClient* ViewPortClient = LocalPlayer->ViewportClient;
    if (!ViewPortClient) {
        logger->log("ERROR", "No UGameViewportClient");
        return CallNextHookEx(NULL, code, wparam, HCBT_CREATEWND);
    };

    void** ViewPortClientVTable = ViewPortClient->VFTable;
    if (!ViewPortClientVTable) {
        logger->log("ERROR", "No ViewPortClientVTable");
        return CallNextHookEx(NULL, code, wparam, HCBT_CREATEWND);
    };

    DWORD protecc;
    VirtualProtect(&ViewPortClientVTable[100], 8, PAGE_EXECUTE_READWRITE, &protecc);
    OPostRender = reinterpret_cast<decltype(OPostRender)>(ViewPortClientVTable[100]);
    ViewPortClientVTable[100] = &PostRender;
    VirtualProtect(&ViewPortClientVTable[100], 8, protecc, 0);

    logger->log("SUCCESS", "Injected");
    logger->log("INFO", std::format("Base Address: 0x{:x}\n", (uintptr_t)GetModuleHandleW(0)).c_str());

    return CallNextHookEx(NULL, code, wparam, lparam);
}