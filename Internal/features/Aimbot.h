#pragma once

/// @file
/// The Aimbot feature: while the aim key is held, locks the aim to the enemy nearest the crosshair
/// (within a FOV radius) and eases the view toward the chosen bone. Reuses the shared ActorCache.
///
/// NOTE: aim origin (eye height), the smoothing curve and the FOV are approximate and can only be
/// tuned in-game; verify on a live match.

#include "Feature.h"
#include "../utils/Globals.h"
#include "../cache/ActorCache.h"
#include "../utils/WorldToScreen.h"

#include <Windows.h>
#include <cmath>

class Aimbot : public Feature
{
  private:
	/// Map the bone selector (0/1/2) to a BoneFNames index.
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

	/// World-direction vector -> view rotation (pitch/yaw in degrees).
	static FRotator DirToRotator(const FVector& d)
	{
		constexpr float toDeg = 180.f / 3.14159265f;
		FRotator r;
		r.Yaw = atan2f(d.Y, d.X) * toDeg;
		r.Pitch = atan2f(d.Z, sqrtf(d.X * d.X + d.Y * d.Y)) * toDeg;
		r.Roll = 0.f;
		return r;
	}

	/// Shortest signed angular delta from @p from to @p to (degrees, wrapped to [-180, 180]).
	static float ShortestAngle(float from, float to)
	{
		return fmodf(to - from + 540.f, 360.f) - 180.f;
	}

  public:
	Aimbot()
	{
		Name = "Aimbot";
		UpdateEnabled();

		Log("Created");
	};

	void UpdateEnabled()
	{
		Enabled = Settings.AIM.Aimbot;
	};

	bool Check()
	{
		if (!Initialized) return false;
		if (!Globals::PlayerController) return false;
		if (!Globals::PlayerController->IsInGame()) return false;
		if (!Globals::Canvas) return false;

		return true;
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
		const auto& aim = Settings.AIM;
		// Silent aim engages while firing (left click) and snaps instantly; otherwise it engages on
		// the aim key with the configured smoothing. (This is a snap-on-fire silent aim, not a
		// trace-redirect one — the view still moves; see docs/roadmap.md.)
		const bool engaged = aim.SilentAim ? (GetAsyncKeyState(VK_LBUTTON) & 0x8000) : (GetAsyncKeyState(aim.AimKey) & 0x8000);
		if (!engaged) return;

		auto* controller = Globals::PlayerController;
		auto* localPawn = controller->AcknowledgedPawn;
		if (!localPawn) return;

		char localTeam = -1;
		if (auto* localChar = reinterpret_cast<APortalWarsCharacter*>(controller->Character))
			localTeam = localChar->GetTeamNum();

		const int bone = BoneIndex(aim.AimBone);
		const FVector2D crosshair{Globals::Canvas->ClipX * 0.5f, Globals::Canvas->ClipY * 0.5f};

		APortalWarsCharacter* target = nullptr;
		FVector targetBone{};
		float best = aim.AimFov;

		for (const auto& cached : ActorCache::Players())
		{
			auto* character = cached.character;
			if (reinterpret_cast<AActor*>(character) == reinterpret_cast<AActor*>(localPawn)) continue;
			if (ActorCache::IsDead(cached)) continue; // don't lock onto a dead body
			if (aim.AimTeamCheck && localTeam >= 0 && cached.team == localTeam) continue;
			if (aim.AimVisibleCheck && !character->WasRecentlyRendered(0.1f)) continue; // only visible targets

			auto* mesh = character->Mesh;
			if (!mesh) continue;

			const FVector2D screen = Projection::Bone(mesh, bone);
			if (!screen.X && !screen.Y) continue; // off-screen / behind camera

			const float dx = screen.X - crosshair.X, dy = screen.Y - crosshair.Y;
			const float d = sqrtf(dx * dx + dy * dy);
			if (d < best)
			{
				best = d;
				target = character;
				targetBone = mesh->GetBoneMatrix(bone); // world position of the same bone
			}
		}

		if (!target) return;

		// Aim origin: the local pawn's location, roughly at eye height.
		FVector eye = localPawn->K2_GetActorLocation();
		eye.Z += 80.f;
		const FVector dir{targetBone.X - eye.X, targetBone.Y - eye.Y, targetBone.Z - eye.Z};
		const FRotator wanted = DirToRotator(dir);

		const FRotator current = controller->ControlRotation;
		float t = aim.SilentAim ? 1.f : aim.AimSmooth; // silent aim snaps instantly
		if (t < 0.01f) t = 0.01f;
		if (t > 1.f) t = 1.f;

		FRotator next;
		next.Pitch = current.Pitch + (wanted.Pitch - current.Pitch) * t;
		next.Yaw = current.Yaw + ShortestAngle(current.Yaw, wanted.Yaw) * t;
		next.Roll = 0.f;

		controller->SetControlRotation(next);
	};
};
