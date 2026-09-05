#pragma once

/// @file
/// The ThirdPersonCam feature: an over-the-shoulder camera placed behind the pawn each frame via
/// ClientSetSpectatorCamera (looking along the aim), a cleaner third person than the console toggle.
///
/// NOTE: like the fly-cam, whether the game keeps our pose can only be confirmed in-game.

#include "Feature.h"
#include "../utils/Globals.h"

#include <cmath>

class ThirdPersonCam : public Feature
{
  public:
	ThirdPersonCam()
	{
		Name = "ThirdPersonCam";
		UpdateEnabled();
		Log("Created");
	};

	void UpdateEnabled()
	{
		Enabled = Settings.EXPLOITS.ThirdPersonCam;
	};

	bool Check()
	{
		return Initialized && Globals::PlayerController && Globals::PlayerController->IsInGame();
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
		auto* pc = Globals::PlayerController;
		auto* pawn = reinterpret_cast<AActor*>(pc->AcknowledgedPawn);
		if (!pawn) return;

		const FRotator rot = pc->ControlRotation;
		constexpr float toRad = 3.14159265f / 180.f;
		const float p = rot.Pitch * toRad, y = rot.Yaw * toRad;
		const FVector forward{cosf(p) * cosf(y), cosf(p) * sinf(y), sinf(p)};

		FVector eye = pawn->K2_GetActorLocation();
		eye.Z += 60.f; // roughly eye height

		const float d = Settings.EXPLOITS.ThirdPersonDistance;
		const float h = Settings.EXPLOITS.ThirdPersonHeight;
		const FVector cam{eye.X - forward.X * d, eye.Y - forward.Y * d, eye.Z - forward.Z * d + h};

		pc->ClientSetSpectatorCamera(cam, rot);
	};
};
