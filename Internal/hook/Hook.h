#pragma once

#include "Features.h"
#include "functions/ProcessEvent.h"
#include "functions/PostRender.h"
#include "../menu/gui/Gui.h"

#include <format>
#include <MinHook.h>

namespace Hook {
	HHOOK g_hook;

	BYTE* SetHook(void** VTable, int index, void* TargetFunction) {
		BYTE* original = reinterpret_cast<BYTE*>(VTable[index]);

		DWORD protecc;
		VirtualProtect(&VTable[index], 8, PAGE_EXECUTE_READWRITE, &protecc);
		VTable[index] = TargetFunction;
		VirtualProtect(&VTable[index], 8, protecc, 0);

		char buffer[256];
		sprintf_s(buffer, sizeof(buffer), "Hooked [%llx] [%llx]", reinterpret_cast<uintptr_t>(VTable[index]), reinterpret_cast<uintptr_t>(&VTable[index]));
		Logger::Log("SUCCESS", buffer);

		return original;
	}; 
	
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

		TArray<ULocalPlayer*>LocalPlayers = OwningGameInstance->LocalPlayers;

		UPortalWarsLocalPlayer* LocalPlayer = (UPortalWarsLocalPlayer*)LocalPlayers[0];
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

		PostRender::VTable = ViewPortClientVTable;
		ProcessEvent::VTable = *reinterpret_cast<void***>(UObject::GetDefaultObj());

		UPortalWarsSaveGame* UserSave = LocalPlayer->GetUserSaveGame();
		if (UserSave) {
			Logger::Log("SUCCESS", "Got user save game");
			Settings.EXPLOITS.FOV = UserSave->FOV;
		};
		
		if (SettingsHelper::Init()) Logger::Log("SUCCESS", std::string("Loaded settings from ").append(SettingsHelper::GetSettingsFilePath()));
		SettingsHelper::Reset(); //Temp fix

		Logger::Log("INFO", std::format("Found [{:d}] Objects", ObjObjects->NumElements));
		
		UObject* NewObject = Globals::GameplayStatics->SpawnObject(UConsole::StaticClass(), Globals::Engine->GameViewport);
		Globals::Engine->GameViewport->ViewportConsole = static_cast<UConsole*>(NewObject);
		Logger::Log("SUCCESS", "UConsole spawned");

		Scripts::Init();

		if (MH_Initialize() != MH_OK)
		{
			Logger::Log("ERROR", "MinHook not initialized");
			return FALSE;
		}

		const auto& ProccessEventTarget = reinterpret_cast<decltype(ProcessEvent::Original)>(ProcessEvent::VTable[ProcessEvent::Index]);
		ProcessEvent::Original = ProccessEventTarget;

		MH_CreateHook(ProccessEventTarget, &ProcessEvent::HookedProcessEvent, reinterpret_cast<void**>(&ProcessEvent::Original));


		PostRender::Original = reinterpret_cast<decltype(PostRender::Original)>(SetHook(PostRender::VTable, PostRender::Index, &PostRender::HookedPostRender));

		if (!GUI::Init())
		{
			Logger::Log("ERROR", "Could not initialize GUI");
			return FALSE;
		}

		if (MH_EnableHook(MH_ALL_HOOKS) != MH_OK)
		{
			Logger::Log("ERROR", "Could not enable MinHook hooks");
			return FALSE;
		};

		Logger::Log("SUCCESS", "Enabled MinHook hooks");

		Features::Init();
		Logger::Log("SUCCESS", std::format("Initialized {} Features", Features::Features.size()));

		return TRUE;
	}

	void UnHook() {
		Logger::Log("INFO", "Unloading");

		if (MH_DisableHook(MH_ALL_HOOKS) != MH_OK)
		{
			Logger::Log("ERROR", "Could not disable MinHook hooks");
		}
		if (MH_Uninitialize() != MH_OK)
		{
			Logger::Log("ERROR", "MinHook uninitialization error");
		}

		SetHook(PostRender::VTable, PostRender::Index, PostRender::Original);
		Logger::DestroyConsole();
		ExceptionHandler::Disable();
		UnhookWindowsHookEx(g_hook);
	}

	bool isKeyPressed(UCHAR key) {
		return GetAsyncKeyState(key) & 1 && GetAsyncKeyState(key) & 0x8000;
	};
};