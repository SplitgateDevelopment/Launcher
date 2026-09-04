#pragma once

/// @file
/// @brief Per-frame actor cache. The visual features (ESP, radar, ...) used to each loop every
/// actor in every level independently; this walks the actor list once per frame and hands out a
/// filtered, validity-checked list of characters they all share.
///
/// It rebuilds every frame (a single pass), which is enough here because IsA is a cheap in-memory
/// class-hierarchy walk. A spawn/despawn-diff cache (std::set_difference on the actor pointers,
/// IsA only on newly-added actors) would avoid even the per-frame IsA, but isn't needed at this
/// actor count — the win is going from N loops to one.

#include <vector>

#include "../ue/Engine.h"
#include "../utils/Globals.h"

/// Shared, once-per-frame view of the world's actors.
namespace ActorCache
{
	inline std::vector<APortalWarsCharacter*> players; ///< valid characters this frame (incl. the local player)
	inline UObject* characterClass = nullptr;		   ///< resolved PortalWarsCharacter class (once)

	/// A cached actor is still safe to use while it isn't hidden or being torn down (the flags UE
	/// sets before destroying an actor).
	inline bool IsValid(AActor* actor)
	{
		return actor && actor->RootComponent && !actor->bHidden && !actor->bActorIsBeingDestroyed;
	}

	/// Rebuild the cache: one pass over every level's actors, keeping valid PortalWarsCharacters.
	/// Call once per frame (from PostRender) before the features run.
	inline void Update()
	{
		players.clear();

		if (!Globals::World) return;
		if (!characterClass) characterClass = ObjObjects->FindObject("Class PortalWars.PortalWarsCharacter");
		if (!characterClass) return;

		auto& Levels = Globals::World->Levels;
		for (int l = 0, levelCount = Levels.Num(); l < levelCount; l++)
		{
			if (!Levels.IsValidIndex(l)) continue;

			ULevel* level = Levels[l];
			if (!level) continue;

			auto& Actors = level->Actors;
			for (int a = 0, actorCount = Actors.Num(); a < actorCount; a++)
			{
				if (!Actors.IsValidIndex(a)) continue;

				AActor* actor = Actors[a];
				if (!IsValid(actor)) continue;
				if (!actor->IsA(characterClass)) continue;

				players.push_back(reinterpret_cast<APortalWarsCharacter*>(actor));
			}
		}
	}

	/// The characters cached this frame. Includes the local player — skip it at the call site.
	inline const std::vector<APortalWarsCharacter*>& Players()
	{
		return players;
	}
} // namespace ActorCache
