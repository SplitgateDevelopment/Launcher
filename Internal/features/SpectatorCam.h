#pragma once

/// @file
/// The SpectatorCam feature: a detached free-flying camera driven by ClientSetSpectatorCamera. While
/// enabled, WASD flies the camera in the view plane, Space/Ctrl move it up/down, and the mouse still
/// aims (the camera looks along ControlRotation). Position resets to the pawn's eye each time it's
/// enabled.
///
/// NOTE: whether the game keeps our camera pose (vs re-asserting its own each frame) and how it
/// interacts with pawn input can only be confirmed in-game. WASD also drives the pawn while focused.

#include "Feature.h"
#include "../utils/Globals.h"
#include "../utils/Input.h"

#include <cmath>

class SpectatorCam : public Feature
{
  private:
	FVector camPos{};
	bool started = false; ///< false until the pose is seeded from the pawn on (re)enable

  public:
	SpectatorCam()
	{
		Name = "SpectatorCam";
		UpdateEnabled();
		Log("Created");
	};

	void UpdateEnabled()
	{
		Enabled = Settings.EXPLOITS.SpectatorCam;
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

	void Destroy()
	{
		started = false; // stop driving the camera; the game resumes its own
	};

	void Run()
	{
		auto* pc = Globals::PlayerController;
		auto* pawn = reinterpret_cast<AActor*>(pc->AcknowledgedPawn);
		const FRotator rot = pc->ControlRotation;

		if (!started)
		{
			camPos = pawn ? pawn->K2_GetActorLocation() : FVector{0.f, 0.f, 0.f};
			camPos.Z += 80.f; // start at eye height
			started = true;
		}

		constexpr float toRad = 3.14159265f / 180.f;
		const float p = rot.Pitch * toRad, y = rot.Yaw * toRad;
		const FVector forward{cosf(p) * cosf(y), cosf(p) * sinf(y), sinf(p)};
		const FVector right{sinf(y), -cosf(y), 0.f}; // horizontal strafe

		const float s = Settings.EXPLOITS.SpectatorCamSpeed;
		if (Input::Down('W')) { camPos.X += forward.X * s; camPos.Y += forward.Y * s; camPos.Z += forward.Z * s; }
		if (Input::Down('S')) { camPos.X -= forward.X * s; camPos.Y -= forward.Y * s; camPos.Z -= forward.Z * s; }
		if (Input::Down('D')) { camPos.X += right.X * s; camPos.Y += right.Y * s; }
		if (Input::Down('A')) { camPos.X -= right.X * s; camPos.Y -= right.Y * s; }
		if (Input::Down(VK_SPACE)) camPos.Z += s;
		if (Input::Down(VK_CONTROL)) camPos.Z -= s;

		pc->ClientSetSpectatorCamera(camPos, rot);
	};
};
