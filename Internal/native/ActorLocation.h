#pragma once

/// @file
/// @brief Actor world-location read that prefers the offset path (RootComponent->RelativeLocation,
/// a plain field read — no ProcessEvent) and falls back to K2_GetActorLocation. Gated by the Debug
/// toggle Settings.DEBUG.NativeActorLocation, and it also falls back whenever the root component
/// isn't available, so it's always safe.

#include "../ue/Engine.h"
#include "../settings/Settings.h"

/// World location of @p actor. RootComponent->RelativeLocation equals the world location for a root
/// component with no parent (players/characters), which is the common case here; K2_GetActorLocation
/// is the fallback for the toggle-off path and for anything without a root component.
inline FVector ActorLocation(AActor* actor)
{
	if (!actor) return FVector{0.f, 0.f, 0.f};

	if (Settings.DEBUG.NativeActorLocation && actor->RootComponent)
		return actor->RootComponent->RelativeLocation;

	return actor->K2_GetActorLocation();
}
