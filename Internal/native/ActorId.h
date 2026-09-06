#pragma once

/// @file
/// @brief Stable per-actor key: the UObject `InternalIndex` (its slot in the GObjects array), a plain
/// field read — no ProcessEvent. It stays constant for the lifetime of the object, so it keys
/// per-actor state across frames (a spawn/despawn diff, cached per-actor data, ...) more reliably
/// than a raw pointer, which can be reused once the actor is freed.

#include <cstdint>

#include "../ue/Engine.h"

/// The actor's stable GObjects slot id, or 0 for a null actor. Accepts any UObject (actors upcast).
inline uint32_t ActorId(const UObject* actor)
{
	return actor ? actor->InternalIndex : 0;
}
