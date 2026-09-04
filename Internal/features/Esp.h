#pragma once

/// @file
/// The Esp feature: per-frame enemy overlay (names, boxes, health bars, bone
/// skeletons, snaplines and distance) drawn onto the game canvas. See the
/// class comment below for scope and the in-game tuning caveats.

#include "Feature.h"
#include "../utils/Globals.h"

#include <cmath>
#include <string>

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
	UObject* CharacterClass = 0; ///< resolved PortalWarsCharacter UClass, used as the actor filter

	/// Convert a settings Color (0-1 RGBA) into the canvas FLinearColor.
	static FLinearColor ToColor(const Color& c) { return FLinearColor{c.R, c.G, c.B, c.A}; }

	/// Treat an exactly-zero projected point as off-screen (world-to-screen
	/// returns {0,0} when the point is behind the camera or failed to project).
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

		const FVector2D tl{left, head.Y}, tr{right, head.Y};
		const FVector2D bl{left, feet.Y}, br{right, feet.Y};

		Globals::Canvas->K2_DrawLine(tl, tr, 1.f, color);
		Globals::Canvas->K2_DrawLine(tr, br, 1.f, color);
		Globals::Canvas->K2_DrawLine(br, bl, 1.f, color);
		Globals::Canvas->K2_DrawLine(bl, tl, 1.f, color);
	}

	// Vertical health bar just left of the box; green (full) to red (empty).
	void DrawHealth(const FVector2D& head, const FVector2D& feet, float health, float maxHealth)
	{
		if (maxHealth <= 0.f) return;

		float pct = health / maxHealth;
		if (pct < 0.f) pct = 0.f;
		if (pct > 1.f) pct = 1.f;

		const float height = feet.Y - head.Y;
		const float width = height * 0.5f;
		const float barX = (head.X + feet.X) * 0.5f - width * 0.5f - 5.f;

		const FLinearColor color{1.f - pct, pct, 0.f, 1.f};
		Globals::Canvas->K2_DrawLine({barX, feet.Y}, {barX, feet.Y - height * pct}, 3.f, color);
	}

	/// Euclidean distance between two world points, returned in metres
	/// (Unreal world units are centimetres).
	static float Distance(const FVector& a, const FVector& b)
	{
		const float dx = a.X - b.X, dy = a.Y - b.Y, dz = a.Z - b.Z;
		return sqrtf(dx * dx + dy * dy + dz * dz) / 100.f; // cm -> m
	}

	/// Draw the bone skeleton by projecting each bone pair to screen space and
	/// connecting them; segments with an off-screen endpoint are skipped.
	template <typename Mesh>
	void DrawSkeleton(Mesh mesh, APlayerController* controller, const FLinearColor& color)
	{
		static constexpr int pairs[][2] = {
			{BoneFNames::head, BoneFNames::neck_01},
			{BoneFNames::neck_01, BoneFNames::spine_03},
			{BoneFNames::spine_03, BoneFNames::spine_01},
			{BoneFNames::spine_01, BoneFNames::pelvis},
			{BoneFNames::spine_03, BoneFNames::upperarm_l},
			{BoneFNames::upperarm_l, BoneFNames::lowerarm_l},
			{BoneFNames::lowerarm_l, BoneFNames::hand_l},
			{BoneFNames::spine_03, BoneFNames::upperarm_r},
			{BoneFNames::upperarm_r, BoneFNames::lowerarm_r},
			{BoneFNames::lowerarm_r, BoneFNames::hand_r},
			{BoneFNames::pelvis, BoneFNames::thigh_l},
			{BoneFNames::thigh_l, BoneFNames::calf_l},
			{BoneFNames::calf_l, BoneFNames::foot_l},
			{BoneFNames::pelvis, BoneFNames::thigh_r},
			{BoneFNames::thigh_r, BoneFNames::calf_r},
			{BoneFNames::calf_r, BoneFNames::foot_r},
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

	/// Resolve the PortalWarsCharacter class once; Initialized stays false (and
	/// the feature idle) until it is found.
	void Init()
	{
		CharacterClass = ObjObjects->FindObject("Class PortalWars.PortalWarsCharacter");

		Initialized = (!!CharacterClass);
		Log("Initialized");
	};

	void Destroy() {
	};

	/// Iterate every actor in every level once, and for each enemy
	/// PortalWarsCharacter draw whatever Settings.VISUALS enables (snapline,
	/// box, health bar, skeleton, name, distance). The local pawn is skipped.
	void Run()
	{
		const auto& visuals = Settings.VISUALS;
		const FLinearColor nameColor = ToColor(visuals.NameColor);
		const FLinearColor boxColor = ToColor(visuals.BoxColor);
		const FLinearColor bonesColor = ToColor(visuals.BonesColor);
		const FLinearColor snaplineColor = ToColor(visuals.SnaplineColor);
		const FLinearColor friendColor = ToColor(visuals.FriendColor);

		auto* controller = Globals::PlayerController;
		auto* localPawn = controller->AcknowledgedPawn;
		const bool hasPlayer = localPawn != nullptr;
		FVector playerPos{};
		if (hasPlayer) playerPos = localPawn->K2_GetActorLocation();

		// The local player's team, so teammates can be filtered/recolored. -1 = unknown (draw all).
		char localTeam = -1;
		if (auto* localChar = reinterpret_cast<APortalWarsCharacter*>(controller->Character))
			localTeam = localChar->GetTeamNum();

		auto& Levels = Globals::World->Levels;
		for (int l = 0, levelCount = Levels.Num(); l < levelCount; l++)
		{
			if (!Levels.IsValidIndex(l)) continue;

			ULevel* Level = Levels[l];
			if (!Level) continue;

			auto& Actors = Level->Actors;
			for (int a = 0, actorCount = Actors.Num(); a < actorCount; a++)
			{
				if (!Actors.IsValidIndex(a)) continue;

				auto Actor = Actors[a];

				if (!Actor) continue;
				if (!Actor->RootComponent) continue;
				if (!Actor->IsA(CharacterClass)) continue;
				if (Actor == localPawn) continue;

				auto Character = reinterpret_cast<APortalWarsCharacter*>(Actor);

				// Team filtering / recoloring: skip teammates unless ShowFriendly, and draw them
				// in FriendColor when shown.
				const char team = Character->GetTeamNum();
				const bool friendly = (localTeam >= 0 && team == localTeam);
				if (friendly && !visuals.ShowFriendly) continue;

				const FLinearColor boxC = friendly ? friendColor : boxColor;
				const FLinearColor bonesC = friendly ? friendColor : bonesColor;
				const FLinearColor snapC = friendly ? friendColor : snaplineColor;
				const FLinearColor nameC = friendly ? friendColor : nameColor;

				auto Mesh = Character->Mesh;

				FVector2D head = Mesh->GetBone(BoneFNames::head, controller);
				FVector2D feet = Mesh->GetBone(BoneFNames::Root, controller);
				if (OffScreen(feet)) continue;

				if (visuals.Snaplines)
					Globals::Canvas->K2_DrawLine({Globals::Canvas->ClipX * 0.5f, Globals::Canvas->ClipY}, feet, 1.f, snapC);

				if (visuals.Box && !OffScreen(head))
					DrawBox(head, feet, boxC);

				if (visuals.Health && !OffScreen(head))
					DrawHealth(head, feet, Character->Health, Character->MaxHealth);

				if (visuals.Bones)
					DrawSkeleton(Mesh, controller, bonesC);

				// Player name from the player state (not the UObject name).
				if (visuals.Name)
				{
					auto* state = Character->PlayerState;
					if (state)
						Globals::Canvas->K2_DrawText(0, state->PlayerNamePrivate, feet, {visuals.FontScale, visuals.FontScale}, nameC, 1.f, {0.f, 0.f, 0.f, 0.f}, {0.f, 0.f}, true, false, true, {0.f, 0.f, 0.f, 1.f});
				}

				if (visuals.Distance && hasPlayer)
				{
					std::string text = std::to_string((int)Distance(playerPos, Actor->K2_GetActorLocation())) + "m";
					Globals::Canvas->K2_DrawText(0, FString(text), {feet.X, feet.Y + 14.f}, {visuals.FontScale, visuals.FontScale}, nameC, 1.f, {0.f, 0.f, 0.f, 0.f}, {0.f, 0.f}, true, false, true, {0.f, 0.f, 0.f, 1.f});
				}
			}
		}
	};
};
