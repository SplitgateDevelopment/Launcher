#pragma once

/// @file
/// The Teleport feature: teleports the local pawn forward along the view direction (toward where
/// you're looking) by a configurable distance when the hotkey is pressed. Driven by the
/// Events::HotKeyPressed edge event (from Input::DispatchHotKeys), so it doesn't poll every frame.

#include "Feature.h"
#include "../ue/Engine.h"
#include "../scripting/Events.h"

#include <cmath>

class Teleport : public Feature
{
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

	// Acts from the HotKeyPressed handler (registered in Init), not the per-frame render loop.
	bool Check()
	{
		return false;
	};

	void Init()
	{
		Initialized = true;
		Events::Register(Events::Type::HotKeyPressed, [](const Events::Payload& p)
						 {
			if (!Settings.EXPLOITS.Teleport) return;
			if (static_cast<int>(p.value) != Settings.EXPLOITS.TeleportKey) return;
			if (!Engine::IsInGame) return;

			auto* pawn = reinterpret_cast<AActor*>(Engine::PlayerController->AcknowledgedPawn);
			if (!pawn) return;

			const FRotator rot = Engine::PlayerController->ControlRotation;
			constexpr float toRad = 3.14159265f / 180.f;
			const float pitch = rot.Pitch * toRad, yaw = rot.Yaw * toRad;
			const FVector forward{cosf(pitch) * cosf(yaw), cosf(pitch) * sinf(yaw), sinf(pitch)};

			const float d = Settings.EXPLOITS.TeleportDistance;
			const FVector loc = pawn->K2_GetActorLocation();
			pawn->K2_TeleportTo(FVector{loc.X + forward.X * d, loc.Y + forward.Y * d, loc.Z + forward.Z * d}, rot); });
		Log("Initialized");
	};

	void Destroy() {
	};

	void Run() {
	};
};
