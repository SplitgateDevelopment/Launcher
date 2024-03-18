#pragma once

#include <format>
#include "Features.h"
#include "../utils/Globals.h"

namespace Hook {
	HHOOK g_hook;

	Features* features = new Features();

	void** PostRenderVTable;
	void(*OriginalPostRender)(UGameViewportClient* UGameViewportClient, Canvas* Canvas) = nullptr;
	int PostRenderIndex = 100;

	void** ProcessEventVTable;
	void (*OriginalProcessEvent)(UObject*, UObject*, void* params) = nullptr;
	int ProcessEventIndex = 68;

	bool Init() {
		Logger::CreateConsole();

		if (!EngineInit()) {
			Logger::Log("ERROR", "No engine init");
			return FALSE;
		};
		Globals::Init();

		if (!Globals::World) {
			Logger::Log("ERROR", "No World");
			return FALSE;
		};

		UGameInstance* OwningGameInstance = Globals::World->OwningGameInstance;
		if (!OwningGameInstance) {
			Logger::Log("ERROR", "No owning game instance");
			return FALSE;
		};

		TArray<UPlayer*>LocalPlayers = OwningGameInstance->LocalPlayers;

		UPlayer* LocalPlayer = LocalPlayers[0];
		if (!LocalPlayer) {
			Logger::Log("ERROR", "No LocalPlayer");
			return FALSE;
		};

		UGameViewportClient* ViewPortClient = LocalPlayer->ViewportClient;
		if (!ViewPortClient) {
			Logger::Log("ERROR", "No UGameViewportClient");
			return FALSE;
		};

		void** ViewPortClientVTable = ViewPortClient->VFTable;
		if (!ViewPortClientVTable) {
			Logger::Log("ERROR", "No ViewPortClientVTable");
			return FALSE;
		};

		PostRenderVTable = ViewPortClientVTable;
		ProcessEventVTable = Globals::World->VFTable;

		UPortalWarsSaveGame* UserSave = ((UPortalWarsLocalPlayer*)LocalPlayer)->GetUserSaveGame();
		if (UserSave) {
			Logger::Log("SUCCESS", "Got user save game");
			Settings.EXPLOITS.FOV = UserSave->FOV;
		};
		
		if (SettingsHelper::Init()) Logger::Log("SUCCESS", std::string("Loaded settings from ").append(SettingsHelper::GetPath()));

		Logger::Log("INFO", format("Found [{:d}] Objects", ObjObjects->NumElements));
		
		UObject* NewObject = Globals::GameplayStatics->SpawnObject(UConsole::StaticClass(), Globals::Engine->GameViewport);
		Globals::Engine->GameViewport->ViewportConsole = static_cast<UConsole*>(NewObject);
		Logger::Log("SUCCESS", "UConsole spawned");

		Scripts::Init();

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
		Logger::Log("SUCCESS", buffer);

		return original;
	};

	void UnHook() {
		Logger::Log("INFO", "Unloading");
		
		SetHook(PostRenderVTable, PostRenderIndex, OriginalPostRender);
		Logger::DestroyConsole();
	}

	bool isKeyPressed(UCHAR key) {
		return GetAsyncKeyState(key) & 1 && GetAsyncKeyState(key) & 0x8000;
	};
};