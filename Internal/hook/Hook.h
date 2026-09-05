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
	// The world-dependent half of Init, run on a worker thread (defined below).
	void InitWorld();

	// Tear the module down exactly once, off the render/UI thread (used by both the Shutdown event
	// and the menu's Unload button — unhooking on the caller's thread would free the trampoline it
	// returns through, or race the render thread).
	inline void RequestUnload()
	{
		static std::atomic<bool> unhooking = false;
		if (!unhooking.exchange(true)) std::thread(&UnHook).detach();
	}

	// Resolve World -> OwningGameInstance -> LocalPlayers[0] -> ViewportClient (+ its VFTable)
	// under SEH so partially-constructed pointers during early injection yield nullptr instead of
	// an access violation. Keep this a leaf helper with no unwinding objects in the __try scope
	// (the TArray copy has no destructor, so it's allowed).
	inline void** TryResolveViewport(UPortalWarsLocalPlayer*& outLocalPlayer, UGameViewportClient*& outViewport)
	{
		__try
		{
			Globals::Init(); // re-resolve the world (it changes across map loads)

			UGameInstance* gameInstance = Globals::World ? Globals::World->OwningGameInstance : nullptr;
			if (!gameInstance) return nullptr;

			TArray<ULocalPlayer*> localPlayers = gameInstance->LocalPlayers;
			if (localPlayers.Num() <= 0 || !localPlayers[0]) return nullptr;

			auto* localPlayer = reinterpret_cast<UPortalWarsLocalPlayer*>(localPlayers[0]);
			auto* viewport = localPlayer->ViewportClient;
			if (!viewport || !viewport->VFTable) return nullptr;

			outLocalPlayer = localPlayer;
			outViewport = viewport;
			return viewport->VFTable;
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			return nullptr;
		}
	}

	bool Init()
	{
		Logger::CreateConsole();

		if (!EngineInit())
		{
			Logger::Log("ERROR", "No engine init");
			return FALSE;
		};
		Globals::Init();

		// The world / local player may not exist yet (a map load, or a menu before the local player
		// is set up). We must NOT wait for them here: this callback runs on the game's UI/message
		// thread, so Sleeping would freeze the game — and the world would never advance while we're
		// asleep. Hand the wait and the rest of init to a detached worker thread; the game thread
		// keeps running, the world becomes ready, and the worker finishes init.
		std::thread(&InitWorld).detach();

		return TRUE;
	}

	// The world-dependent half of init, run on a worker thread (see Init). Polls off-thread for the
	// world / local player, then installs the hooks, GUI and features. Logs and returns on failure.
	void InitWorld()
	{
		UPortalWarsLocalPlayer* LocalPlayer = nullptr;
		UGameViewportClient* ViewPortClient = nullptr;
		void** ViewPortClientVTable = nullptr;

		// Off-thread, so waiting generously (up to ~60s) stalls nothing.
		for (int attempt = 0; attempt < 600; ++attempt)
		{
			ViewPortClientVTable = TryResolveViewport(LocalPlayer, ViewPortClient);
			if (ViewPortClientVTable) break;

			LocalPlayer = nullptr;
			ViewPortClient = nullptr;

			if (attempt == 0)
				Logger::Log("INFO", "Waiting for the world / local player to be ready...");

			Sleep(100);
		}

		if (!ViewPortClientVTable)
		{
			Logger::Log("ERROR", "World / local player never became ready");
			return;
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
			return;
		}

		const auto& ProccessEventTarget = reinterpret_cast<decltype(ProcessEvent::Original)>(ProcessEvent::VTable[ProcessEvent::Index]);
		ProcessEvent::Original = ProccessEventTarget;

		MH_CreateHook(ProccessEventTarget, &ProcessEvent::HookedProcessEvent, reinterpret_cast<void**>(&ProcessEvent::Original));

		PostRender::Original = reinterpret_cast<decltype(PostRender::Original)>(SetHook(PostRender::VTable, PostRender::Index, &PostRender::HookedPostRender));

		if (!GUI::Init())
		{
			Logger::Log("ERROR", "Could not initialize GUI");
			return;
		}

		if (MH_EnableHook(MH_ALL_HOOKS) != MH_OK)
		{
			Logger::Log("ERROR", "Could not enable MinHook hooks");
			return;
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
						 { RequestUnload(); });
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

		// Reset the module state so a later re-injection re-runs Init cleanly instead of the callback
		// bailing on a stale g_initialized (or re-registering features/handlers on top of the old set).
		Features::Features.clear();
		Events::Clear();
		g_initialized = false;
	}

	bool isKeyPressed(UCHAR key)
	{
		return GetAsyncKeyState(key) & 1 && GetAsyncKeyState(key) & 0x8000;
	};
}; // namespace Hook