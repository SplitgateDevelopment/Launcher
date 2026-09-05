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
#include "../utils/ActorLocation.h"

/// Shared, once-per-frame view of the world's actors.
namespace ActorCache
{
	/// A character plus the per-frame values every visual feature needs, each resolved with a
	/// ProcessEvent — cached here so ESP and radar don't each re-fetch them.
	struct Player
	{
		APortalWarsCharacter* character;
		FVector location; ///< K2_GetActorLocation, once
		char team;		  ///< GetTeamNum, once (-1 if unknown)
		float health;	  ///< Health field, once (<= 0 = dead body on the ground)
		bool isBot;		  ///< PlayerState->bIsABot, once (an AI bot, not a real player)
	};

	/// A cached player is dead (a corpse on the ground) once its health drops to zero. Features that
	/// track live enemies (ESP, aimbot, triggerbot) skip these; radar still shows them.
	inline bool IsDead(const Player& p)
	{
		return p.health <= 0.f;
	}

	inline std::vector<Player> players;		  ///< cached characters this frame (incl. the local player)
	inline UObject* characterClass = nullptr; ///< resolved PortalWarsCharacter class (once)

	/// A cached actor is still safe to use while it isn't hidden or being torn down (the flags UE
	/// sets before destroying an actor).
	inline bool IsValid(AActor* actor)
	{
		return actor && actor->RootComponent && !actor->bHidden && !actor->bActorIsBeingDestroyed;
	}

	/// Do the actual actor pass unconditionally: one loop over every level's actors, keeping valid
	/// PortalWarsCharacters and caching their location + team + health. Used by Update() (gated) and by
	/// the scripting Actors module (which needs the list regardless of the feature toggles).
	inline void Rebuild()
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

				auto* character = reinterpret_cast<APortalWarsCharacter*>(actor);
				const auto* state = character->PlayerState;
				players.push_back({character, ActorLocation(reinterpret_cast<AActor*>(character)), character->GetTeamNum(), character->Health, state && state->bIsABot});
			}
		}
	}

	/// Rebuild the cache for this frame, but only when something consumes it: the visual features
	/// (ESP, radar) or the aim features (aimbot, triggerbot). Call once per frame (from PostRender)
	/// before the features run. Skips the pass entirely when nothing needs it.
	inline void Update()
	{
		if (!Settings.VISUALS.Esp && !Settings.VISUALS.Radar && !Settings.VISUALS.GlowEnemy && !Settings.VISUALS.GlowFriendly && !Settings.AIM.Aimbot && !Settings.AIM.Triggerbot)
		{
			players.clear();
			return;
		}
		Rebuild();
	}

	/// The characters cached this frame. Includes the local player — skip it at the call site.
	inline const std::vector<Player>& Players()
	{
		return players;
	}
} // namespace ActorCache
