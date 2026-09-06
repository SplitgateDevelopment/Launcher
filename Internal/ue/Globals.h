#pragma once

#include <atomic>

#include "Engine.h"

/**
 * @file
 * @brief Cached pointers to the key engine objects, resolved once at startup so features
 * don't re-walk the object graph each frame.
 *
 * Declarations only; the pointers and Globals::Init are defined in Globals.cpp.
 */
namespace Globals
{
	extern UEngine* Engine;							  ///< the global UEngine
	extern UWorld* World;							  ///< the current UWorld
	extern APortalWarsPlayerController* PlayerController; ///< local player controller (set elsewhere as it changes)
	extern UGameplayStatics* GameplayStatics;		  ///< UGameplayStatics CDO
	extern UCanvas* Canvas;							  ///< draw canvas (set during rendering)

	/// Cached PlayerController->IsInGame(), refreshed by PostRender on the game thread. The external
	/// overlay renders the menu on its own thread; it reads this flag instead of dereferencing the
	/// controller, which the game may have freed mid map-load (a null check can't catch a freed object).
	extern std::atomic<bool> IsInGame;

	/// Resolves the engine/world and the GameplayStatics CDO. Call once after the engine is up.
	void Init();
} // namespace Globals
