#pragma once

/// @file
/// The Teleport feature: teleports the local pawn forward along the view direction (toward where
/// you're looking) by a configurable distance when the hotkey is pressed.

#include "Feature.h"
#include "../utils/Globals.h"
#include "../utils/Input.h"

#include <cmath>

class Teleport : public Feature
{
  private:
	bool wasDown = false; ///< edge tracking so each key press teleports once

  public:
	Teleport()
	{
		Name = "Teleport";
		UpdateEnabled();
		Log("Created");
	};

	void UpdateEnabled()
	{
		Enabled = Settings.EXPLOITS.Teleport;
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
		wasDown = false;
	};

	void Run()
	{
		const bool down = Input::Down(Settings.EXPLOITS.TeleportKey);
		if (down && !wasDown)
		{
			auto* pawn = reinterpret_cast<AActor*>(Globals::PlayerController->AcknowledgedPawn);
			if (pawn)
			{
				const FRotator rot = Globals::PlayerController->ControlRotation;
				constexpr float toRad = 3.14159265f / 180.f;
				const float pitch = rot.Pitch * toRad, yaw = rot.Yaw * toRad;
				const FVector forward{cosf(pitch) * cosf(yaw), cosf(pitch) * sinf(yaw), sinf(pitch)};

				const float d = Settings.EXPLOITS.TeleportDistance;
				const FVector loc = pawn->K2_GetActorLocation();
				pawn->K2_TeleportTo(FVector{loc.X + forward.X * d, loc.Y + forward.Y * d, loc.Z + forward.Z * d}, rot);
			}
		}
		wasDown = down;
	};
};
