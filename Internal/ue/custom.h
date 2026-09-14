#pragma once

/// @file
/// @brief Hand-written free helpers layered on top of the generated SDK.
/// Declarations only; the bodies (and the hand-added SDK member methods) live in custom.cpp.

#include "sdk/Fwd.h"
#include "sdk/Enums.h"

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
