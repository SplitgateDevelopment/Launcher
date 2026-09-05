#pragma once

/// @file
/// The BulletTp feature ("bullet teleport" / projectile aimbot): each frame, teleports the local
/// player's own live projectiles onto the aim bone of the enemy nearest the crosshair, so shots home
/// onto the target. Only the local player's projectiles are moved (filtered by Instigator).

#include "Feature.h"
#include "../utils/Globals.h"
#include "../cache/ActorCache.h"
#include "../native/WorldToScreen.h"

#include <cmath>

class BulletTp : public Feature
{
  private:
	UObject* projectileClass = nullptr; ///< resolved PortalWars.Projectile class (matches subclasses via IsA)

	/// Map the aim bone selector (0/1/2) to a BoneFNames index.
	static int BoneIndex(int selector)
	{
		switch (selector)
		{
		case 1:
			return BoneFNames::spine_03;
		case 2:
			return BoneFNames::pelvis;
		default:
			return BoneFNames::head;
		}
	}

  public:
	BulletTp()
	{
		Name = "BulletTp";
		UpdateEnabled();
		Log("Created");
	};

	void UpdateEnabled()
	{
		Enabled = Settings.AIM.BulletTp;
	};

	bool Check()
	{
		if (!Initialized) return false;
		if (!projectileClass) return false;
		if (!Globals::PlayerController || !Globals::PlayerController->IsInGame()) return false;
		if (!Globals::World || !Globals::Canvas) return false;
		return true;
	};

	void Init()
	{
		projectileClass = ObjObjects->FindObject("Class PortalWars.Projectile");
		Initialized = true;
		Log("Initialized");
	};

	void Destroy() {
	};

	void Run()
	{
		const auto& aim = Settings.AIM;
		auto* controller = Globals::PlayerController;
		auto* localPawn = controller->AcknowledgedPawn;
		if (!localPawn) return;

		char localTeam = -1;
		if (auto* localChar = reinterpret_cast<APortalWarsCharacter*>(controller->Character))
			localTeam = localChar->GetTeamNum();

		// 1. Pick the target: the enemy whose aim bone is nearest the crosshair.
		const int bone = BoneIndex(aim.AimBone);
		const FVector2D crosshair{Globals::Canvas->ClipX * 0.5f, Globals::Canvas->ClipY * 0.5f};

		FVector targetBone{};
		bool haveTarget = false;
		float best = 1e9f;
		for (const auto& cached : ActorCache::Players())
		{
			auto* character = cached.character;
			if (reinterpret_cast<AActor*>(character) == reinterpret_cast<AActor*>(localPawn)) continue;
			if (ActorCache::IsDead(cached)) continue;
			if (aim.AimTeamCheck && localTeam >= 0 && cached.team == localTeam) continue;
			if (aim.IgnoreBots && cached.isBot) continue;

			auto* mesh = character->Mesh;
			if (!mesh) continue;

			const FVector2D screen = Projection::Bone(mesh, bone);
			if (!screen.X && !screen.Y) continue; // off-screen

			const float dx = screen.X - crosshair.X, dy = screen.Y - crosshair.Y;
			const float d = sqrtf(dx * dx + dy * dy);
			if (d < best)
			{
				best = d;
				targetBone = mesh->GetBoneMatrix(bone);
				haveTarget = true;
			}
		}
		if (!haveTarget) return;

		// 2. Teleport every one of our own live projectiles onto that bone.
		auto& levels = Globals::World->Levels;
		for (int l = 0, levelCount = levels.Num(); l < levelCount; l++)
		{
			if (!levels.IsValidIndex(l)) continue;
			ULevel* level = levels[l];
			if (!level) continue;

			auto& actors = level->Actors;
			for (int a = 0, actorCount = actors.Num(); a < actorCount; a++)
			{
				if (!actors.IsValidIndex(a)) continue;
				AActor* actor = actors[a];
				if (!actor || actor->bActorIsBeingDestroyed) continue;
				if (!actor->IsA(projectileClass)) continue;
				if (reinterpret_cast<AActor*>(actor->Instigator) != reinterpret_cast<AActor*>(localPawn)) continue; // only our shots

				FHitResult hit{};
				actor->K2_SetActorLocation(targetBone, false, hit, true);
			}
		}
	};
};
