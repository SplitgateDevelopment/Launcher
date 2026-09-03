#pragma once

#include "../ue/Engine.h"

/**
 * @file
 * @brief Cached pointers to the key engine objects, resolved once at startup so features
 * don't re-walk the object graph each frame.
 */
namespace Globals
{
	UEngine* Engine = 0;							   ///< the global UEngine
	UWorld* World = 0;								   ///< the current UWorld
	APortalWarsPlayerController* PlayerController = 0; ///< local player controller (set elsewhere as it changes)
	UGameplayStatics* GameplayStatics = 0;			   ///< UGameplayStatics CDO
	UKismetStringLibrary* KismetStringLibrary = 0;	   ///< UKismetStringLibrary CDO
	UKismetTextLibrary* KismetTextLibrary = 0;		   ///< UKismetTextLibrary CDO
	UCanvas* Canvas = 0;							   ///< draw canvas (set during rendering)

	/// Resolves the engine/world and the static library CDOs. Call once after the engine is up.
	void Init()
	{
		Engine = UEngine::GetEngine();
		World = UWorld::GetWorld();
		GameplayStatics = reinterpret_cast<UGameplayStatics*>(UGameplayStatics::StaticClass());
		KismetStringLibrary = reinterpret_cast<UKismetStringLibrary*>(UKismetStringLibrary::StaticClass());
		KismetTextLibrary = reinterpret_cast<UKismetTextLibrary*>(UKismetTextLibrary::StaticClass());
	}
} // namespace Globals