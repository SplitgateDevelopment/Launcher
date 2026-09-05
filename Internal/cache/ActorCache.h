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
#include "../native/ActorLocation.h"

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
		int kills;		  ///< PlayerState->PlayerStats.Kills, once (0 if no player state)
		int deaths;		  ///< PlayerState->PlayerStats.Deaths, once
		int killstreak;	  ///< PlayerState->KillStreak, once (current life's streak)
		int rank;		  ///< highest PlayerRanks[].RankLevel, once (0 if none)
	};

	/// A cached player is dead (a corpse on the ground) once its health drops to zero. Features that
	/// track live enemies (ESP, aimbot, triggerbot) skip these; radar still shows them.
	inline bool IsDead(const Player& p)
	{
		return p.health <= 0.f;
	}

	inline std::vector<Player> players;			///< cached characters this frame (incl. the local player)
	inline std::vector<AActor*> projectiles;	///< live projectiles this frame (for BulletTraces / BulletTp)
	inline UObject* characterClass = nullptr;	///< resolved PortalWarsCharacter class (once)
	inline UObject* projectileClass = nullptr;	///< resolved PortalWars.Projectile class (once)

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
		projectiles.clear();

		if (!Globals::World) return;
		if (!characterClass) characterClass = ObjObjects->FindObject("Class PortalWars.PortalWarsCharacter");
		if (!characterClass) return;

		// Collect projectiles in the same pass only when a projectile feature needs them.
		const bool wantProjectiles = Settings.VISUALS.BulletTraces || Settings.EXPLOITS.BulletTp;
		if (wantProjectiles && !projectileClass) projectileClass = ObjObjects->FindObject("Class PortalWars.Projectile");

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
				if (!actor || actor->bActorIsBeingDestroyed) continue;

				// Projectiles (a projectile isn't a character, so classify and move on).
				if (wantProjectiles && projectileClass && actor->RootComponent && actor->IsA(projectileClass))
				{
					projectiles.push_back(actor);
					continue;
				}

				if (!IsValid(actor)) continue;
				if (!actor->IsA(characterClass)) continue;

				auto* character = reinterpret_cast<APortalWarsCharacter*>(actor);
				// PlayerState is an APortalWarsPlayerState at runtime; cast up for the bot flag + stats.
				auto* state = reinterpret_cast<APortalWarsPlayerState*>(character->PlayerState);

				int rank = 0;
				if (state && Settings.VISUALS.Rank) // only pay the PlayerRanks read when the ESP rank label is on
					for (int r = 0, rn = state->PlayerRanks.Num(); r < rn; r++)
						if (state->PlayerRanks[r].RankLevel > rank) rank = state->PlayerRanks[r].RankLevel;

				players.push_back({character,
								   ActorLocation(reinterpret_cast<AActor*>(character)),
								   character->GetTeamNum(),
								   character->Health,
								   state && state->bIsABot,
								   state ? state->PlayerStats.Kills : 0,
								   state ? state->PlayerStats.Deaths : 0,
								   state ? static_cast<int>(state->KillStreak) : 0,
								   rank});
			}
		}
	}

	/// Rebuild the cache for this frame, but only when something consumes it: the visual features
	/// (ESP, radar) or the aim features (aimbot, triggerbot). Call once per frame (from PostRender)
	/// before the features run. Skips the pass entirely when nothing needs it.
	inline void Update()
	{
		// Only scan during a live match. Out of a game (menu) or in the post-match lobby
		// (APortalWarsPostPlayerController) the actor list is large and nothing consumes the cache,
		// so the per-frame walk is pure cost — this is the post-game FPS drop. Features already skip
		// via their own IsInGame() Check(), so clearing here is safe.
		auto* pc = Globals::PlayerController;
		if (!pc || !pc->IsInGame() || IsPostGameController(reinterpret_cast<UObject*>(pc)))
		{
			players.clear();
			projectiles.clear();
			return;
		}

		const auto& v = Settings.VISUALS;
		const auto& aim = Settings.AIM;
		if (!v.Esp && !v.Radar && !v.GlowEnemy && !v.GlowFriendly && !v.BulletTraces && !aim.Aimbot && !aim.Triggerbot && !Settings.EXPLOITS.BulletTp)
		{
			players.clear();
			projectiles.clear();
			return;
		}
		Rebuild();
	}

	/// The characters cached this frame. Includes the local player — skip it at the call site.
	inline const std::vector<Player>& Players()
	{
		return players;
	}

	/// The live projectiles cached this frame (only populated when BulletTraces / BulletTp is on).
	inline const std::vector<AActor*>& Projectiles()
	{
		return projectiles;
	}
} // namespace ActorCache
