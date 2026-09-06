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

/// Engine bootstrap surface: the signature-resolved globals and the one-time init.
namespace Engine
{
	extern FNamePool* NamePoolData;	  ///< the FName pool, resolved by Init()
	extern TUObjectArray* ObjObjects; ///< the global UObject array, resolved by Init()
	extern UWorld* WRLD;			  ///< address of the game's UWorld* slot, resolved by Init()
	extern uintptr_t GetBoneMatrixF;  ///< scanned GetBoneMatrix function pointer (bone projection)

	/// One-time bootstrap: resolve the globals from byte signatures. Returns false if any fails.
	/// (UFunctions are resolved lazily at each wrapper's first call via a function-local static.)
	bool Init();
} // namespace Engine
