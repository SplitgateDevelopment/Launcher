// dllmain.cpp : Definisce il punto di ingresso per l'applicazione DLL.

#include <Windows.h>
#include <iostream>
#include <format>
#include "Engine.h"

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
		
		if (GetAsyncKeyState(VK_INSERT)) {
			MessageBoxA(0, "Loading map", "INFO", MB_OK);
			PlayerController->SwitchLevel(L"Simulation_Alpha?gameMode=SHOTSNIP");
		};
	} while (false);

	OPostRender(UGameViewportClient, canvas);
}

__declspec(dllexport) LRESULT CALLBACK SplitgateCallBack(int code, WPARAM wparam, LPARAM lparam) {
    MSG* msg = (MSG*)lparam;
    if (msg->message != (69 * 420)) return CallNextHookEx(0, code, wparam, lparam);

    if (!EngineInit()) {
        MessageBoxA(0, "No engine init", "ERROR", MB_OK);
        return CallNextHookEx(0, code, wparam, lparam);
    };

    UWorld* World = *(UWorld**)(WRLD);
    if (!World) {
        MessageBoxA(0, "No World", "ERROR", MB_OK);
        return CallNextHookEx(0, code, wparam, lparam);
    };

    UGameInstance* OwningGameInstance = World->OwningGameInstance;
    if (!OwningGameInstance) {
        MessageBoxA(0, "No owning game instance", "ERROR", MB_OK);
        return CallNextHookEx(0, code, wparam, lparam);
    };

    TArray<UPlayer*>LocalPlayers = OwningGameInstance->LocalPlayers;

    UPlayer* LocalPlayer = LocalPlayers[0];
    if (!LocalPlayer) {
        MessageBoxA(0, "No LocalPlayer", "ERROR", MB_OK);
        return CallNextHookEx(0, code, wparam, lparam);
    };

    UGameViewportClient* ViewPortClient = LocalPlayer->ViewportClient;
    if (!ViewPortClient) {
        MessageBoxA(0, "No UGameViewportClient", "ERROR", MB_OK);
        return CallNextHookEx(0, code, wparam, lparam);
    };

    void** ViewPortClientVTable = ViewPortClient->VFTable;
    if (!ViewPortClientVTable) {
        MessageBoxA(0, "No ViewPortClientVTable", "ERROR", MB_OK);
        return CallNextHookEx(0, code, wparam, lparam);
    };

    DWORD protecc;
    VirtualProtect(&ViewPortClientVTable[100], 8, PAGE_EXECUTE_READWRITE, &protecc);
    OPostRender = reinterpret_cast<decltype(OPostRender)>(ViewPortClientVTable[100]);
    ViewPortClientVTable[100] = &PostRender;
    VirtualProtect(&ViewPortClientVTable[100], 8, protecc, 0);

    MessageBoxA(0, "Injected", "SUCCESS", MB_OK);
    MessageBoxA(0, std::format("Base Address: 0x{:x}\n", (uintptr_t)GetModuleHandleW(0)).c_str(), "INFO", MB_OK);

    return CallNextHookEx(0, code, wparam, lparam);
}