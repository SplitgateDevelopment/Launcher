#pragma once

#include "Feature.h"
#include "../utils/Globals.h"

// Player ESP: draws names, boxes and/or bone skeletons for enemy characters,
// configured in the Visuals menu tab (Settings.VISUALS). Filters to
// PortalWarsCharacter actors, so non-players are never drawn. Iterates the
// actor list once per frame and draws whatever is enabled.
//
// NOTE: this is rendering code that can only be tuned in-game. Box proportions,
// the bone pairs and the 3D-box mode may need adjustment; verify on a live game.
class Esp : public Feature
{
private:
	UObject* CharacterClass = 0;

	static FLinearColor ToColor(const Color& c) { return FLinearColor{ c.R, c.G, c.B, c.A }; }

	static bool OffScreen(const FVector2D& p) { return !p.X && !p.Y; }

	// 2D bounding box from the head (top) down to the root/feet (bottom).
	// TODO: a true 3D box needs a world-to-screen for the box corners, which the
	// SDK does not expose yet; Box3D currently draws the same 2D box.
	void DrawBox(const FVector2D& head, const FVector2D& feet, const FLinearColor& color)
	{
		const float height = feet.Y - head.Y;
		const float width = height * 0.5f;
		const float centerX = (head.X + feet.X) * 0.5f;
		const float left = centerX - width * 0.5f;
		const float right = centerX + width * 0.5f;

		const FVector2D tl{ left, head.Y }, tr{ right, head.Y };
		const FVector2D bl{ left, feet.Y }, br{ right, feet.Y };

		Globals::Canvas->K2_DrawLine(tl, tr, 1.f, color);
		Globals::Canvas->K2_DrawLine(tr, br, 1.f, color);
		Globals::Canvas->K2_DrawLine(br, bl, 1.f, color);
		Globals::Canvas->K2_DrawLine(bl, tl, 1.f, color);
	}

	template <typename Mesh>
	void DrawSkeleton(Mesh mesh, APlayerController* controller, const FLinearColor& color)
	{
		static constexpr int pairs[][2] = {
			{ BoneFNames::head, BoneFNames::neck_01 },
			{ BoneFNames::neck_01, BoneFNames::spine_03 },
			{ BoneFNames::spine_03, BoneFNames::spine_01 },
			{ BoneFNames::spine_01, BoneFNames::pelvis },
			{ BoneFNames::spine_03, BoneFNames::upperarm_l },
			{ BoneFNames::upperarm_l, BoneFNames::lowerarm_l },
			{ BoneFNames::lowerarm_l, BoneFNames::hand_l },
			{ BoneFNames::spine_03, BoneFNames::upperarm_r },
			{ BoneFNames::upperarm_r, BoneFNames::lowerarm_r },
			{ BoneFNames::lowerarm_r, BoneFNames::hand_r },
			{ BoneFNames::pelvis, BoneFNames::thigh_l },
			{ BoneFNames::thigh_l, BoneFNames::calf_l },
			{ BoneFNames::calf_l, BoneFNames::foot_l },
			{ BoneFNames::pelvis, BoneFNames::thigh_r },
			{ BoneFNames::thigh_r, BoneFNames::calf_r },
			{ BoneFNames::calf_r, BoneFNames::foot_r },
		};

		for (const auto& pair : pairs)
		{
			FVector2D a = mesh->GetBone(pair[0], controller);
			FVector2D b = mesh->GetBone(pair[1], controller);
			if (OffScreen(a) || OffScreen(b)) continue;

			Globals::Canvas->K2_DrawLine(a, b, 1.f, color);
		}
	}

public:
	Esp()
	{
		Name = "Esp";
		UpdateEnabled();

		Log("Created");
	};

	void UpdateEnabled()
	{
		Enabled = Settings.VISUALS.Esp;
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
		const auto& visuals = Settings.VISUALS;
		const FLinearColor nameColor = ToColor(visuals.NameColor);
		const FLinearColor boxColor = ToColor(visuals.BoxColor);
		const FLinearColor bonesColor = ToColor(visuals.BonesColor);

		auto* controller = Globals::PlayerController;
		auto* localPawn = controller->AcknowledgedPawn;

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
				if (!Actor->RootComponent) continue;
				if (!Actor->IsA(CharacterClass)) continue;
				if (Actor == localPawn) continue;

				auto Character = reinterpret_cast<APortalWarsCharacter*>(Actor);
				auto Mesh = Character->Mesh;

				FVector2D head = Mesh->GetBone(BoneFNames::head, controller);
				FVector2D feet = Mesh->GetBone(BoneFNames::Root, controller);
				if (OffScreen(feet)) continue;

				if (visuals.Name)
					Globals::Canvas->K2_DrawText(0, Actor->GetName(), feet, { 1.f, 1.f }, nameColor, 1.f, { 0.f, 0.f, 0.f, 0.f }, { 0.f, 0.f }, true, false, true, { 0.f, 0.f, 0.f, 1.f });

				if (visuals.Box && !OffScreen(head))
					DrawBox(head, feet, boxColor);

				if (visuals.Bones)
					DrawSkeleton(Mesh, controller, bonesColor);
			}
		}
	};
};
