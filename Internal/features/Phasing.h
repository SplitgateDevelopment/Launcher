#pragma once

/// @file
/// The Phasing (wallbang) feature: disables collision on the map's cover actors so hitscan shots
/// pass through them. Approach A from the roadmap — no native fire hook needed. See the class
/// comment for the scoping and caveats.

#include "Feature.h"
#include "../utils/Globals.h"
#include "../ue/Engine.h"

#include <functional>

// Phasing bullets: cycles the actor list and turns off collision on every `CullableActor` (the
// game's destructible cover / wall pieces), so a hitscan trace passes straight through them. Scoped
// to CullableActor on purpose — the base floor/BSP is left colliding, so you don't fall through the
// map — and gun pickups (`ABaseGun : ACullableActor`) are skipped so phasing doesn't disable weapon
// pickups. Re-enables collision on toggle-off.
//
// NOTE: the server may still validate hits, so this is most useful against the self-hosted backend;
// and "which class is a wall" is a heuristic — verify in-game that cover phases and movement is
// unaffected. Only-while-firing scoping and the native trace-redirect version are noted in the
// roadmap.
class Phasing : public Feature
{
  private:
	UObject* cullableClass = nullptr; ///< resolved PortalWars.CullableActor UClass (the cover filter)
	UObject* baseGunClass = nullptr;  ///< resolved PortalWars.BaseGun UClass (excluded: it derives from CullableActor)

	int scanTimer = 0;					   ///< frames until the next rescan (0 = scan this frame)
	static constexpr int RescanInterval = 30; ///< rescan cadence, to catch streamed-in geometry without a per-frame walk

	/// Run @p fn for every currently-valid CullableActor that isn't a gun. Walks the live actor list,
	/// so it never touches a despawned pointer (safe for both the disable pass and the restore pass).
	void ForEachCover(const std::function<void(AActor*)>& fn)
	{
		if (!Globals::World) return;

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
				if (!actor->IsA(cullableClass)) continue;
				if (baseGunClass && actor->IsA(baseGunClass)) continue; // don't disable weapon pickups
				fn(actor);
			}
		}
	}

  public:
	Phasing()
	{
		Name = "Phasing";
		UpdateEnabled();

		Log("Created");
	};

	void UpdateEnabled()
	{
		Enabled = Settings.EXPLOITS.PhasingBullets;
	};

	bool Check()
	{
		if (!Initialized) return false;
		if (!Globals::PlayerController) return false;
		if (!Globals::PlayerController->IsInGame()) return false;
		if (!Globals::World) return false;

		return true;
	};

	/// Resolve the cover class once; stays idle until PortalWars.CullableActor is found.
	void Init()
	{
		cullableClass = Engine::ObjObjects->FindObject("Class PortalWars.CullableActor");
		baseGunClass = Engine::ObjObjects->FindObject("Class PortalWars.BaseGun");

		Initialized = (!!cullableClass);
		Log("Initialized");
	};

	/// Re-enable collision on all current cover on the disable edge.
	void Destroy()
	{
		ForEachCover([](AActor* actor) { actor->SetActorEnableCollision(true); });
		scanTimer = 0; // rescan immediately if re-enabled
	};

	/// Disable cover collision, rescanning every RescanInterval frames (idempotent, so re-asserting
	/// false is cheap) to catch geometry streamed in after the feature was enabled.
	void Run()
	{
		if (scanTimer > 0)
		{
			scanTimer--;
			return;
		}
		scanTimer = RescanInterval;

		ForEachCover([](AActor* actor) { actor->SetActorEnableCollision(false); });
	};
};
