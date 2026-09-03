#pragma once

#include "Feature.h"
#include "../utils/Globals.h"

#include <cmath>

// A simple 2D radar drawn in the top-right of the screen: enemy players are
// plotted relative to the local player's position and facing (forward = up).
//
// NOTE: rendering + world math that can only be tuned in-game — the rotation
// mapping (and its sign) and the range/size constants may need adjusting on a
// live game.
class Radar : public Feature
{
private:
	UObject* CharacterClass = 0;

	static constexpr float Size = 200.f;    // radar panel size, px
	static constexpr float Margin = 20.f;   // distance from the screen edge, px
	static constexpr float Range = 6000.f;  // world units mapped to the radar radius

	void Cross(float x, float y, float half, float thickness, const FLinearColor& color)
	{
		Globals::Canvas->K2_DrawLine({ x - half, y }, { x + half, y }, thickness, color);
		Globals::Canvas->K2_DrawLine({ x, y - half }, { x, y + half }, thickness, color);
	}

public:
	Radar()
	{
		Name = "Radar";
		UpdateEnabled();

		Log("Created");
	};

	void UpdateEnabled()
	{
		Enabled = Settings.VISUALS.Radar;
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
		CharacterClass = ObjObjects->FindObject("Class PortalWars.PortalWarsCharacter");

		Initialized = (!!CharacterClass);
		Log("Initialized");
	};

	void Destroy()
	{
	};

	void Run()
	{
		auto* controller = Globals::PlayerController;
		auto* localPawn = controller->AcknowledgedPawn;
		if (!localPawn) return;

		const FVector playerPos = localPawn->K2_GetActorLocation();
		const float yaw = controller->ControlRotation.Yaw * (3.14159265f / 180.f);
		const float cosYaw = cosf(yaw);
		const float sinYaw = sinf(yaw);

		const float radius = Size * 0.5f;
		const float cx = Globals::Canvas->ClipX - Margin - radius;
		const float cy = Margin + radius;

		const FLinearColor border{ 1.f, 1.f, 1.f, 1.f };
		const FLinearColor dot{ 1.f, 0.f, 0.f, 1.f };

		// Panel border + player marker at the centre.
		const FVector2D tl{ cx - radius, cy - radius }, tr{ cx + radius, cy - radius };
		const FVector2D bl{ cx - radius, cy + radius }, br{ cx + radius, cy + radius };
		Globals::Canvas->K2_DrawLine(tl, tr, 1.f, border);
		Globals::Canvas->K2_DrawLine(tr, br, 1.f, border);
		Globals::Canvas->K2_DrawLine(br, bl, 1.f, border);
		Globals::Canvas->K2_DrawLine(bl, tl, 1.f, border);
		Cross(cx, cy, 3.f, 1.f, border);

		auto& Levels = Globals::World->Levels;
		for (int l = 0, levelCount = Levels.Num(); l < levelCount; l++) {
			if (!Levels.IsValidIndex(l)) continue;

			ULevel* Level = Levels[l];
			if (!Level) continue;

			auto& Actors = Level->Actors;
			for (int a = 0, actorCount = Actors.Num(); a < actorCount; a++) {
				if (!Actors.IsValidIndex(a)) continue;

				auto Actor = Actors[a];
				if (!Actor) continue;
				if (!Actor->IsA(CharacterClass)) continue;
				if (Actor == localPawn) continue;

				const FVector enemyPos = Actor->K2_GetActorLocation();
				const float dx = enemyPos.X - playerPos.X;
				const float dy = enemyPos.Y - playerPos.Y;

				// Rotate the world delta into player-relative space (forward/right).
				const float forward = dx * cosYaw + dy * sinYaw;
				const float right = -dx * sinYaw + dy * cosYaw;

				const float px = cx + (right / Range) * radius;
				const float py = cy - (forward / Range) * radius;  // forward = up

				if (px < cx - radius || px > cx + radius || py < cy - radius || py > cy + radius) continue;

				Cross(px, py, 2.f, 2.f, dot);
			}
		}
	};
};
