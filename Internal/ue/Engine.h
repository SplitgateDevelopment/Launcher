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

// --- Engine globals, free helpers and init (namespaced in later phases) ---

/// Spawn an actor of @p actorClass at @p location via the deferred two-step
/// (BeginDeferredActorSpawnFromClass -> FinishSpawningActor). Returns the actor, or nullptr.
struct AActor* SpawnActor(struct UObject* worldContextObject, struct UClass* actorClass, struct FVector location, ESpawnActorCollisionHandlingMethod collision, struct AActor* owner);

/**
 * Line-of-sight test between two world points via UKismetSystemLibrary::LineTraceSingle on the
 * visibility channel. @p ignoreActor (the target) is excluded from the trace, so a point on that
 * actor counts as visible when nothing else blocks the ray to it.
 *
 * @param worldContext any live UObject that resolves the world (e.g. the local PlayerController).
 * @param start        ray origin (typically the shooter's eye / camera).
 * @param end          ray end (the bone / point being tested).
 * @param ignoreActor  actor excluded from the trace (nullptr to ignore nothing).
 * @return true if @p end is reachable (no blocking geometry between it and @p start). Fails open
 *         (returns true) when the trace UFunction or world context isn't available, so callers that
 *         gate on visibility never lock up if the probe can't run.
 *
 * NOTE: uses trace channel 0 (ETraceTypeQuery::TraceTypeQuery1 = Visibility by default). If the
 * game remaps that channel, this is the value to verify in-game.
 */
bool LineTraceVisible(struct UObject* worldContext, const struct FVector& start, const struct FVector& end, struct AActor* ignoreActor);

/**
 * Whether @p controller is the post-match lobby controller (`APortalWarsPostPlayerController`).
 * After a game ends the local player controller becomes this type; the heavy per-frame actor scan
 * (ActorCache) should stop there to avoid the post-game FPS drop. False for null / not-yet-resolvable.
 */
bool IsPostGameController(struct UObject* controller);

extern FNamePool* NamePoolData;

extern TUObjectArray* ObjObjects;

extern UWorld* WRLD;

bool EngineInit();
