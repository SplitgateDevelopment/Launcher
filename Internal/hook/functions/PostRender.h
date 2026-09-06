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

	/// @brief Resolve this frame's local player controller: World → OwningGameInstance →
	/// LocalPlayers[0] → PlayerController, bailing out (nullptr) at the first missing link — e.g. mid
	/// map-load, before the local player exists.
	/// @param outWorld receives the resolved world on success; left untouched otherwise.
	/// @return the local APortalWarsPlayerController, or nullptr if the walk can't complete.
	inline APortalWarsPlayerController* ResolvePlayerController(UWorld*& outWorld)
	{
		UWorld* world = UWorld::GetWorld();
		if (!world) return nullptr;

		UGameInstance* gameInstance = world->OwningGameInstance;
		if (!gameInstance) return nullptr;

		TArray<ULocalPlayer*> localPlayers = gameInstance->LocalPlayers;
		if (localPlayers.Num() <= 0) return nullptr;

		auto* localPlayer = (UPortalWarsLocalPlayer*)localPlayers[0];
		if (!localPlayer) return nullptr;

		APlayerController* controller = localPlayer->PlayerController;
		if (!controller) return nullptr;

		outWorld = world;
		return (APortalWarsPlayerController*)controller;
	}

	/// @brief Hooked PostRender: refreshes globals, runs features, then forwards.
	/// @param UGameViewportClient The viewport client issuing the frame.
	/// @param Canvas The canvas features draw onto this frame.
	void HookedPostRender(UGameViewportClient* UGameViewportClient, UCanvas* Canvas)
	{
		UWorld* World = nullptr;
		APortalWarsPlayerController* PlayerController = ResolvePlayerController(World);

		// Publish the shared controller + cached IsInGame every frame — null/false when the walk fails
		// (e.g. mid map-load, once the old controller is destroyed) rather than leaving a stale pointer:
		// the external overlay reads these from its own thread, so a dangling pointer here is an
		// off-thread fault (a null check can't detect a freed object). IsInGame is evaluated here on the
		// game thread, where the controller is valid.
		Engine::PlayerController = PlayerController;
		Engine::IsInGame = PlayerController && PlayerController->IsInGame();

		if (PlayerController)
		{
			Engine::World = World;
			Engine::Canvas = Canvas;

			// Edge-detect hotkeys once per frame → Events::HotKeyPressed (press-once actions subscribe).
			Input::DispatchHotKeys();

			// One shared actor pass per frame, consumed by the visual features below.
			ActorCache::Update();

			// Point the drawing backend at the selected renderer for this frame (features draw
			// through Render::*). ImGui-recorded commands are replayed in the Present hook.
			Render::Select(Settings.VISUALS.Renderer);

			Features::Execute();
		}

		return Original(UGameViewportClient, Canvas);
	}
} // namespace PostRender