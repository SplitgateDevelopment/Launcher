#pragma once

/// @file
/// The BulletSpeed feature: each frame, pushes the local player's own projectiles further along their
/// current velocity, so shots travel faster. Reuses the shared projectile pass (ActorCache).
///
/// NOTE: the boost is per-frame, so effective speed scales with frame rate — tune the slider to taste.

#include "Feature.h"
#include "../utils/Globals.h"
#include "../cache/ActorCache.h"

#include <cmath>

class BulletSpeed : public Feature
{
  public:
	BulletSpeed()
	{
		Name = "BulletSpeed";
		UpdateEnabled();
		Log("Created");
	};

	void UpdateEnabled()
	{
		Enabled = Settings.EXPLOITS.BulletSpeed;
	};

	bool Check()
	{
		return Initialized && Globals::PlayerController && Globals::PlayerController->IsInGame() && Globals::World;
	};

	void Init()
	{
		Initialized = true;
		Log("Initialized");
	};

	void Destroy() {
	};

	void Run()
	{
		auto* localPawn = reinterpret_cast<AActor*>(Globals::PlayerController->AcknowledgedPawn);
		if (!localPawn) return;

		const float boost = Settings.EXPLOITS.BulletSpeedBoost;
		for (AActor* actor : ActorCache::Projectiles())
		{
			// Only our own shots (Instigator or Owner is the local pawn).
			if (reinterpret_cast<AActor*>(actor->Instigator) != localPawn && actor->Owner != localPawn) continue;

			const FVector v = actor->GetVelocity();
			const float len = sqrtf(v.X * v.X + v.Y * v.Y + v.Z * v.Z);
			if (len < 1.f) continue; // not moving yet

			const FVector dir{v.X / len, v.Y / len, v.Z / len};
			const FVector pos = actor->K2_GetActorLocation();
			FHitResult hit{};
			actor->K2_SetActorLocation(FVector{pos.X + dir.X * boost, pos.Y + dir.Y * boost, pos.Z + dir.Z * boost}, false, hit, true);
		}
	};
};
