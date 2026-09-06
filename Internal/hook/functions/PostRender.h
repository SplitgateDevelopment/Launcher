#pragma once

/// @file
/// @brief Hooked UGameViewportClient::PostRender — the DLL's per-frame tick.
///
/// PostRender runs once per rendered frame, making it the natural place to
/// drive the cheat each frame. The hook resolves the current world, local
/// player and player controller, caches them in Globals for features to use,
/// runs Features::Execute() (which also renders the menu/overlay), then
/// forwards to the original so the game keeps drawing normally.
#include "../../ue/Engine.h"
#include "../../settings/Settings.h"
#include "../../cache/ActorCache.h"
#include "../../render/Render.h"
#include "../../features/Features.h"
#include "../../utils/Input.h"

/// @brief Hook code for UGameViewportClient::PostRender.
namespace PostRender
{
	void** VTable;																		   ///< VTable the hook is installed into.
	void (*Original)(UGameViewportClient* UGameViewportClient, UCanvas* Canvas) = nullptr; ///< Trampoline to the original PostRender.
	int Index = 100;																	   ///< VTable index of PostRender to swap.

	/// @brief Hooked PostRender: refreshes globals, runs features, then forwards.
	/// @param UGameViewportClient The viewport client issuing the frame.
	/// @param Canvas The canvas features draw onto this frame.
	void HookedPostRender(UGameViewportClient* UGameViewportClient, UCanvas* Canvas)
	{
		do
		{
			// Clear the shared controller (and the cached IsInGame flag) when the walk fails (e.g. mid
			// map-load, once the old controller is destroyed) instead of leaving it dangling: the external
			// overlay renders the menu on its own thread and reads these, so a stale pointer here is an
			// off-thread fault (the menu guards against null, but it can't detect a freed object).
			UWorld* World = UWorld::GetWorld();
			if (!World) { Engine::PlayerController = nullptr; Engine::IsInGame = false; break; }

			UGameInstance* OwningGameInstance = World->OwningGameInstance;
			if (!OwningGameInstance) { Engine::PlayerController = nullptr; Engine::IsInGame = false; break; }

			TArray<ULocalPlayer*> LocalPlayers = OwningGameInstance->LocalPlayers;

			UPortalWarsLocalPlayer* LocalPlayer = (UPortalWarsLocalPlayer*)LocalPlayers[0];
			if (!LocalPlayer) { Engine::PlayerController = nullptr; Engine::IsInGame = false; break; }

			APlayerController* PlayerController = LocalPlayer->PlayerController;
			if (!PlayerController) { Engine::PlayerController = nullptr; Engine::IsInGame = false; break; }

			Engine::World = World;
			Engine::Canvas = Canvas;
			Engine::PlayerController = (APortalWarsPlayerController*)PlayerController;
			// Refresh the cached flag here on the game thread, where the controller is valid, so the
			// off-thread menu can read it without touching the (possibly freed) controller itself.
			Engine::IsInGame = PlayerController->IsInGame();

			// Edge-detect hotkeys once per frame → Events::HotKeyPressed (press-once actions subscribe).
			Input::DispatchHotKeys();

			// One shared actor pass per frame, consumed by the visual features below.
			ActorCache::Update();

			// Point the drawing backend at the selected renderer for this frame (features draw
			// through Render::*). ImGui-recorded commands are replayed in the Present hook.
			Render::Select(Settings.VISUALS.Renderer);

			Features::Execute();
		} while (false);

		return Original(UGameViewportClient, Canvas);
	}
} // namespace PostRender