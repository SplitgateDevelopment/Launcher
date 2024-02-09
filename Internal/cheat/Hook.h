#pragma once

#include <format>
#include "Features.h"

class Hook {

public:
	HHOOK g_hook;

	Features* features = new Features();
	Logger* logger = features->logger;

	void** PostRenderVTable;
	void(*OriginalPostRender)(UGameViewportClient* UGameViewportClient, Canvas* Canvas) = nullptr;
	int PostRenderIndex = 100;

	void** ProcessEventVTable;
	void (*OriginalProcessEvent)(UObject*, UObject*, void* params) = nullptr;
	int ProcessEventIndex = 68;

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

		PostRenderVTable = ViewPortClientVTable;
		ProcessEventVTable = World->VFTable;

		if (SettingsHelper::Init()) logger->log("SUCCESS", std::string("Loaded settings from ").append(SettingsHelper::GetPath()));

		UPortalWarsSaveGame* UserSave = ((UPortalWarsLocalPlayer*)LocalPlayer)->GetUserSaveGame();
		if (UserSave) {
			logger->log("SUCCESS", "Got user save game");
			Settings.EXPLOITS.FOV = UserSave->FOV;
		};

		logger->log("INFO", format("Found [{:d}] Objects", ObjObjects->NumElements));

		return TRUE;
	}

	BYTE* SetHook(void** VTable, int index, void* TargetFunction) {
		BYTE* original = reinterpret_cast<BYTE*>(VTable[index]);

		DWORD protecc;
		VirtualProtect(&VTable[index], 8, PAGE_EXECUTE_READWRITE, &protecc);
		VTable[index] = TargetFunction;
		VirtualProtect(&VTable[index], 8, protecc, 0);

		char buffer[256];
		sprintf_s(buffer, sizeof(buffer), "Hooked [%llx] [%llx]", VTable[index], reinterpret_cast<uintptr_t>(&VTable[index]));
		logger->log("SUCCESS", buffer);

		return original;
	};

	void UnHook() {
		logger->log("INFO", "Unloading");
		
		SetHook(PostRenderVTable, PostRenderIndex, OriginalPostRender);
		logger->DestroyConsole();
	}

	bool isKeyPressed(UCHAR key) {
		return GetAsyncKeyState(key) & 1 && GetAsyncKeyState(key) & 0x8000;
	};
};