#pragma once

/// @file
/// @brief Reverse-engineered Unreal Engine SDK for Splitgate (PortalWars).
///
/// Large, machine-generated header declaring the engine and game classes,
/// structs, enums and UFunction wrappers the hooks and features call into
/// (FName pool, UObject layout, actors, controllers, canvas, etc.). Generated
/// from the game binary rather than hand-written; treat it as an opaque
/// dependency and do not edit by hand.
#include <Windows.h>
#include <cstdint>
#include <string>
#include <vector>
#include <iostream>
#include <atomic>

#include "sdk/Fwd.h"
#include "sdk/Enums.h"
#include "sdk/Values.h"

#include "sdk/UObject.h"
#include "sdk/UField.h"
#include "sdk/UStruct.h"
#include "sdk/UClass.h"
#include "sdk/UFunction.h"
#include "sdk/AActor.h"
#include "sdk/AController.h"
#include "sdk/APawn.h"
#include "sdk/ACharacter.h"
#include "sdk/APortalWarsCharacter.h"
#include "sdk/ACharacterSkin.h"
#include "sdk/AInfo.h"
#include "sdk/APlayerState.h"
#include "sdk/APortalWarsPlayerState.h"
#include "sdk/AHUD.h"
#include "sdk/APlayerController.h"
#include "sdk/ACullableActor.h"
#include "sdk/ABaseGun.h"
#include "sdk/AGun.h"
#include "sdk/ULevel.h"
#include "sdk/UGameInstance.h"
#include "sdk/UPortalWarsGameInstance.h"
#include "sdk/UPlayer.h"
#include "sdk/ULocalPlayer.h"
#include "sdk/UPortalWarsLocalPlayer.h"
#include "sdk/USaveGame.h"
#include "sdk/UPortalWarsSaveGame.h"
#include "sdk/APlayerCameraManager.h"
#include "sdk/UActorComponent.h"
#include "sdk/USceneComponent.h"
#include "sdk/UPrimitiveComponent.h"
#include "sdk/UMeshComponent.h"
#include "sdk/USkinnedMeshComponent.h"
#include "sdk/USkeletalMeshComponent.h"
#include "sdk/UStaticMeshComponent.h"
#include "sdk/UWorld.h"
#include "sdk/UFont.h"
#include "sdk/UCanvas.h"
#include "sdk/UScriptViewportClient.h"
#include "sdk/UGameViewportClient.h"
#include "sdk/UBlueprintFunctionLibrary.h"
#include "sdk/UGameplayStatics.h"
#include "sdk/UEngine.h"
#include "sdk/UInputSettings.h"
#include "sdk/UConsole.h"
#include "sdk/UKismetStringLibrary.h"
#include "sdk/UKismetTextLibrary.h"
#include "sdk/UPortalWarsNotificationManager.h"
#include "sdk/APortalWarsBasePlayerController.h"
#include "sdk/APortalWarsPlayerController.h"
#include "sdk/UNetConnection.h"
#include "sdk/UNetDriver.h"

// Hand-written free helpers layered on the generated SDK (bodies in custom.cpp).
#include "custom.h"

/// Engine runtime state: the signature-scanned internals, the resolved game objects,
/// and the two bootstrap steps that fill them in.
namespace Engine
{
	// --- Signature-scanned internals (raw addresses, resolved once by Init) ---
	inline FNamePool* GNames = nullptr;		  ///< the FName pool
	inline TUObjectArray* GObjects = nullptr; ///< the global UObject array
	inline UWorld* GWorld = nullptr;		  ///< address of the game's UWorld* slot
	inline uintptr_t GetBoneMatrixFn = 0;	  ///< scanned GetBoneMatrix function pointer (bone projection)

	// --- Resolved game objects (cached pointers, filled by ResolveObjects) ---
	inline UEngine* GEngine = nullptr;						 ///< the global UEngine
	inline UWorld* World = nullptr;							 ///< the current UWorld (resolved snapshot)
	inline APortalWarsPlayerController* PlayerController = nullptr; ///< local player controller (updated as it changes)
	inline UGameplayStatics* GameplayStatics = nullptr;		 ///< UGameplayStatics CDO
	inline UKismetStringLibrary* KismetStringLibrary = nullptr; ///< UKismetStringLibrary CDO
	inline UKismetTextLibrary* KismetTextLibrary = nullptr;	 ///< UKismetTextLibrary CDO
	inline UCanvas* Canvas = nullptr;						 ///< draw canvas (set during rendering)

	/// Cached PlayerController->IsInGame(), refreshed by PostRender on the game thread. The external
	/// overlay renders the menu on its own thread; it reads this flag instead of dereferencing the
	/// controller, which the game may have freed mid map-load (a null check can't catch a freed object).
	inline std::atomic<bool> IsInGame = false;

	/// One-time bootstrap: resolve the scanned internals from byte signatures. Returns false if
	/// any signature fails. (UFunctions are resolved lazily at each wrapper's first call.)
	bool Init();

	/// Resolve GEngine/World and the static-library CDOs. Call after the engine is up and again
	/// on each map load (the world changes). Does not touch PlayerController/Canvas (set per-frame).
	void ResolveObjects();
} // namespace Engine
