#pragma once

#include "../features/Features.h"
#include "functions/ProcessEvent.h"
#include "functions/PostRender.h"
#include "../menu/gui/Gui.h"

#include <atomic>
#include <format>
#include <thread>
#include <MinHook.h>

#pragma comment(lib, "MinHook.x64.lib")

namespace Hook
{
	inline HHOOK g_hook;
	inline bool g_initialized = false;

	BYTE* SetHook(void** VTable, int index, void* TargetFunction)
	{
		BYTE* original = reinterpret_cast<BYTE*>(VTable[index]);

		DWORD protect, oldProtect;

		VirtualProtect(&VTable[index], 8, PAGE_EXECUTE_READWRITE, &protect);
		VTable[index] = TargetFunction;
		VirtualProtect(&VTable[index], 8, protect, &oldProtect);

		char buffer[256];
		sprintf_s(buffer, sizeof(buffer), "Hooked [%llx] [%llx]", reinterpret_cast<uintptr_t>(VTable[index]), reinterpret_cast<uintptr_t>(&VTable[index]));
		Logger::Log("SUCCESS", buffer);

		return original;
	};

	// Defined below; forward-declared so Init can register it as the shutdown handler.
	void UnHook();

	bool Init()
	{
		Logger::CreateConsole();

		if (!EngineInit())
		{
			Logger::Log("ERROR", "No engine init");
			return FALSE;
		};
		Globals::Init();

		// Injection can land before the world / local player exist (during a map load, or at a
		// menu before the local player is set up), which used to make Init bail immediately and
		// surface to the launcher as a "DLL failed to initialize (timed out)". Instead, poll the
		// whole chain until it's ready — bounded to ~10s, under the launcher's 15s handshake
		// timeout, so a genuine failure still eventually surfaces as a timeout.
		UPortalWarsLocalPlayer* LocalPlayer = nullptr;
		UGameViewportClient* ViewPortClient = nullptr;
		void** ViewPortClientVTable = nullptr;

		for (int attempt = 0; attempt < 100; ++attempt)
		{
			Globals::Init(); // re-resolve the world (it changes across map loads)

			UGameInstance* OwningGameInstance = Globals::World ? Globals::World->OwningGameInstance : nullptr;
			if (OwningGameInstance)
			{
				TArray<ULocalPlayer*> LocalPlayers = OwningGameInstance->LocalPlayers;
				if (LocalPlayers.Num() > 0 && LocalPlayers[0]) // guard against an empty array (OOB read)
				{
					LocalPlayer = (UPortalWarsLocalPlayer*)LocalPlayers[0];
					ViewPortClient = LocalPlayer->ViewportClient;
					ViewPortClientVTable = ViewPortClient ? ViewPortClient->VFTable : nullptr;
					if (ViewPortClientVTable) break;
				}
			}

			LocalPlayer = nullptr;
			ViewPortClient = nullptr;
			ViewPortClientVTable = nullptr;

			if (attempt == 0)
			{
				Logger::Log("INFO", "Waiting for the world / local player to be ready...");
			}

			Sleep(100);
		}

		if (!ViewPortClientVTable)
		{
			Logger::Log("ERROR", "World / local player never became ready");
			return FALSE;
		};

		PostRender::VTable = ViewPortClientVTable;
		ProcessEvent::VTable = *reinterpret_cast<void***>(UObject::GetDefaultObj());

		UPortalWarsSaveGame* UserSave = LocalPlayer->GetUserSaveGame();
		if (UserSave)
		{
			Logger::Log("SUCCESS", "Got user save game");
			Settings.EXPLOITS.FOV = UserSave->FOV;
		};

		if (SettingsHelper::File().Load())
		{
			auto settingsPath = SettingsHelper::File().Path().string();
			Logger::Log("SUCCESS", std::string("Loaded settings from ").append(settingsPath));
		}

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

		// Tear our hooks down when the game shuts down (Events::Type::Shutdown is dispatched
		// from ProcessEvent when GameInstance.ReceiveShutdown fires). Deferred to a detached
		// thread: the event runs inside ProcessEvent, and unhooking there would free the
		// trampoline we return through. Guarded against a double teardown.
		// NOTE: UnHook also destroys the GUI, which can race the render thread; acceptable
		// during shutdown but wants in-game verification.
		Events::Register(Events::Type::Shutdown, []
						 {
			static std::atomic<bool> unhooking = false;
			if (!unhooking.exchange(true)) std::thread(&UnHook).detach(); });

		return TRUE;
	}

	void UnHook()
	{
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
		GUI::Destroy();

		if (g_hook)
		{
			UnhookWindowsHookEx(g_hook);
			g_hook = nullptr;
		}
	}

	bool isKeyPressed(UCHAR key)
	{
		return GetAsyncKeyState(key) & 1 && GetAsyncKeyState(key) & 0x8000;
	};
}; // namespace Hook