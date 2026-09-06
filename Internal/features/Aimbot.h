#pragma once

/// @file
/// The Aimbot feature: while the aim key is held, locks the aim to the enemy nearest the crosshair
/// (within a FOV radius) and eases the view toward the chosen bone. Reuses the shared ActorCache.
///
/// NOTE: aim origin (eye height), the smoothing curve and the FOV are approximate and can only be
/// tuned in-game; verify on a live match.

#include "Feature.h"
#include "../ue/Engine.h"
#include "../cache/ActorCache.h"
#include "../native/WorldToScreen.h"
#include "../native/Visibility.h"
#include "../utils/Input.h"

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

	/// First bone in line of sight from @p eye, trying the configured bone then head/chest/pelvis;
	/// returns its BoneFNames index, or -1 when none is visible. One line trace per tested bone
	/// (the target itself is ignored), so only called in the strict per-bone visibility mode.
	static int FirstVisibleBone(USkeletalMeshComponent* mesh, APortalWarsCharacter* character, const FVector& eye, int selector)
	{
		const int candidates[] = {BoneIndex(selector), BoneFNames::head, BoneFNames::spine_03, BoneFNames::pelvis};
		for (int b : candidates)
		{
			const FVector world = mesh->GetBoneMatrix(b);
			if (LineTraceVisible(Engine::PlayerController, eye, world, reinterpret_cast<AActor*>(character)))
				return b;
		}
		return -1;
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
		if (!Engine::PlayerController) return false;
		if (!Engine::PlayerController->IsInGame()) return false;
		if (!Engine::Canvas) return false;

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
		const bool engaged = aim.SilentAim ? Input::Down(VK_LBUTTON) : Input::Down(aim.AimKey);
		if (!engaged) return;

		auto* controller = Engine::PlayerController;
		auto* localPawn = controller->AcknowledgedPawn;
		if (!localPawn) return;

		char localTeam = -1;
		if (auto* localChar = reinterpret_cast<APortalWarsCharacter*>(controller->Character))
			localTeam = localChar->GetTeamNum();

		const int bone = BoneIndex(aim.AimBone);
		const FVector2D crosshair{Engine::Canvas->ClipX * 0.5f, Engine::Canvas->ClipY * 0.5f};

		// Aim origin: the local pawn's location, roughly at eye height. Also the start point for the
		// strict per-bone line-of-sight traces below.
		FVector eye = localPawn->K2_GetActorLocation();
		eye.Z += 80.f;

		// Strict per-bone visibility: line-trace each bone and aim at the first one in line of sight,
		// instead of the coarse whole-actor WasRecentlyRendered gate.
		const bool perBone = aim.AimVisibleCheck && aim.AimVisiblePerBone;

		APortalWarsCharacter* target = nullptr;
		FVector targetBone{};
		float best = aim.AimFov;

		for (const auto& cached : ActorCache::Players())
		{
			auto* character = cached.character;
			if (reinterpret_cast<AActor*>(character) == reinterpret_cast<AActor*>(localPawn)) continue;
			if (ActorCache::IsDead(cached)) continue; // don't lock onto a dead body
			if (aim.AimTeamCheck && localTeam >= 0 && cached.team == localTeam) continue;
			if (aim.IgnoreBots && cached.isBot) continue; // target only real players
			if (aim.AimVisibleCheck && !perBone && !Visibility::IsVisible(character, eye)) continue; // only visible targets

			auto* mesh = character->Mesh;
			if (!mesh) continue;

			// Pick the bone to aim at: the configured one, or (strict mode) the first bone in line
			// of sight; a target with no visible bone is skipped entirely.
			int useBone = bone;
			if (perBone)
			{
				useBone = FirstVisibleBone(mesh, character, eye, aim.AimBone);
				if (useBone < 0) continue;
			}

			const FVector2D screen = Projection::Bone(mesh, useBone);
			if (!screen.X && !screen.Y) continue; // off-screen / behind camera

			const float dx = screen.X - crosshair.X, dy = screen.Y - crosshair.Y;
			const float d = sqrtf(dx * dx + dy * dy);
			if (d < best)
			{
				best = d;
				target = character;
				targetBone = mesh->GetBoneMatrix(useBone); // world position of the chosen bone
			}
		}

		if (!target) return;

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
