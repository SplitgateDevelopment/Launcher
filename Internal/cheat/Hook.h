#pragma once

#include <format>
#include "Features.h"

class Hook {

public:
	HHOOK g_hook;
	Features* features = new Features();
	void** VTABLE;
	Logger* logger = features->logger;

	bool Init() {
		if (!EngineInit()) {
			logger->log("ERROR", "No engine init");
			return FALSE;
		};

		UWorld* World = *(UWorld**)(WRLD);
		if (!World) {
			logger->log("ERROR", "No World");
			return FALSE;
		};

		UGameInstance* OwningGameInstance = World->OwningGameInstance;
		if (!OwningGameInstance) {
			logger->log("ERROR", "No owning game instance");
			return FALSE;
		};

		TArray<UPlayer*>LocalPlayers = OwningGameInstance->LocalPlayers;

		UPlayer* LocalPlayer = LocalPlayers[0];
		if (!LocalPlayer) {
			logger->log("ERROR", "No LocalPlayer");
			return FALSE;
		};

		UGameViewportClient* ViewPortClient = LocalPlayer->ViewportClient;
		if (!ViewPortClient) {
			logger->log("ERROR", "No UGameViewportClient");
			return FALSE;
		};

		void** ViewPortClientVTable = ViewPortClient->VFTable;
		if (!ViewPortClientVTable) {
			logger->log("ERROR", "No ViewPortClientVTable");
			return FALSE;
		};

		VTABLE = ViewPortClientVTable;
		return TRUE;
	}

	void swapRender(void** ViewPortClientVTable, void (*NPostRender)(UGameViewportClient* UGameViewportClient, Canvas* canvas)) {

		DWORD protecc;
		VirtualProtect(&ViewPortClientVTable[100], 8, PAGE_EXECUTE_READWRITE, &protecc);
		ViewPortClientVTable[100] = NPostRender;
		VirtualProtect(&ViewPortClientVTable[100], 8, protecc, 0);

		logger->log("INFO", "Swapped PostRender functions");
	}

	bool isKeyPressed(UCHAR key) {
		return GetAsyncKeyState(key) & 1 && GetAsyncKeyState(key) & 0x8000;
	};
};