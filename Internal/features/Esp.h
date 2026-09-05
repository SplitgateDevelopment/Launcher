#pragma once

/// @file
/// The Esp feature: per-frame enemy overlay (names, boxes, health bars, bone
/// skeletons, snaplines and distance) drawn onto the game canvas. See the
/// class comment below for scope and the in-game tuning caveats.

#include "Feature.h"
#include "../utils/Globals.h"
#include "../cache/ActorCache.h"
#include "../native/WorldToScreen.h"
#include "../native/Visibility.h"
#include "../utils/Rgb.h"
#include "../render/Render.h"

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

	// 2D bounding box from the head (top) down to the root/feet (bottom). The 3D variant is
	// DrawBox3D below.
	void DrawBox(const FVector2D& head, const FVector2D& feet, const FLinearColor& color)
	{
		const float height = feet.Y - head.Y;
		const float width = height * 0.5f;
		const float centerX = (head.X + feet.X) * 0.5f;
		const float left = centerX - width * 0.5f;
		const float right = centerX + width * 0.5f;

		const FVector2D tl{left, head.Y}, tr{right, head.Y};
		const FVector2D bl{left, feet.Y}, br{right, feet.Y};

		Render::Line(tl, tr, 1.f, color);
		Render::Line(tr, br, 1.f, color);
		Render::Line(br, bl, 1.f, color);
		Render::Line(bl, tl, 1.f, color);
	}

	// A true 3D box: project the 8 corners of a world-space box centred on the actor and connect
	// them. Extents are approximate character half-sizes (cm) and may want in-game tuning.
	void DrawBox3D(APlayerController* controller, const FVector& origin, const FLinearColor& color)
	{
		static constexpr float ex = 34.f, ey = 34.f, ez = 92.f; // half-extents (x, y, z)

		const FVector world[8] = {
			{origin.X - ex, origin.Y - ey, origin.Z - ez},
			{origin.X + ex, origin.Y - ey, origin.Z - ez},
			{origin.X + ex, origin.Y + ey, origin.Z - ez},
			{origin.X - ex, origin.Y + ey, origin.Z - ez},
			{origin.X - ex, origin.Y - ey, origin.Z + ez},
			{origin.X + ex, origin.Y - ey, origin.Z + ez},
			{origin.X + ex, origin.Y + ey, origin.Z + ez},
			{origin.X - ex, origin.Y + ey, origin.Z + ez},
		};

		FVector2D screen[8];
		for (int i = 0; i < 8; i++)
			if (!Projection::WorldToScreen(world[i], screen[i])) return; // a corner behind the camera

		static constexpr int edges[][2] = {
			{0, 1},
			{1, 2},
			{2, 3},
			{3, 0}, // bottom face
			{4, 5},
			{5, 6},
			{6, 7},
			{7, 4}, // top face
			{0, 4},
			{1, 5},
			{2, 6},
			{3, 7}, // verticals
		};
		for (const auto& e : edges)
			Render::Line(screen[e[0]], screen[e[1]], 1.f, color);
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
		Render::Line({barX, feet.Y}, {barX, feet.Y - height * pct}, 3.f, color);
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
			FVector2D a = Projection::Bone(mesh, pair[0]);
			FVector2D b = Projection::Bone(mesh, pair[1]);
			if (OffScreen(a) || OffScreen(b)) continue;

			Render::Line(a, b, 1.f, color);
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
		const FLinearColor visibleColor = ToColor(visuals.VisibleColor);

		// RGB overrides the box/bone/snapline colors (for enemies and teammates alike) with the
		// cycling rainbow, computed once per frame. Name and health keep their own colors.
		const bool rgb = Settings.MENU.Rgb;
		const FLinearColor rgbColor = rgb ? ToColor(Rgb::Current()) : FLinearColor{};

		auto* controller = Globals::PlayerController;
		auto* localPawn = controller->AcknowledgedPawn;
		const bool hasPlayer = localPawn != nullptr;
		FVector playerPos{};
		if (hasPlayer) playerPos = localPawn->K2_GetActorLocation();

		// The local player's team, so teammates can be filtered/recolored. -1 = unknown (draw all).
		char localTeam = -1;
		if (auto* localChar = reinterpret_cast<APortalWarsCharacter*>(controller->Character))
			localTeam = localChar->GetTeamNum();

		// One shared actor pass built this frame in ActorCache; iterate the cached characters
		// (location + team already resolved there, so no per-feature ProcessEvent for them).
		for (const auto& cached : ActorCache::Players())
		{
			auto* Character = cached.character;
			if (reinterpret_cast<AActor*>(Character) == reinterpret_cast<AActor*>(localPawn)) continue;
			if (ActorCache::IsDead(cached)) continue; // stop drawing a dead body on the ground

			// Team filtering / recoloring: skip teammates unless ShowFriendly, and draw them
			// in FriendColor when shown.
			const char team = cached.team;
			const bool friendly = (localTeam >= 0 && team == localTeam);
			if (friendly && !visuals.ShowFriendly) continue;

			// Distance cull: skip players past the configured range (0 = unlimited). Reused below.
			const float dist = hasPlayer ? Distance(playerPos, cached.location) : 0.f;
			if (visuals.MaxDistance > 0.f && hasPlayer && dist > visuals.MaxDistance) continue;

			// Visibility recolor: an enemy that was recently rendered (not occluded) is drawn in
			// VisibleColor; occluded enemies keep the normal box/bone/snapline colors. One
			// ProcessEvent per enemy, so only paid when the check is on and the enemy isn't a teammate.
			const bool visible = (visuals.EspVisibleCheck && !friendly) ? Visibility::IsVisible(Character, 0.06f) : false;
			const FLinearColor enemyBox = visible ? visibleColor : boxColor;
			const FLinearColor enemyBones = visible ? visibleColor : bonesColor;
			const FLinearColor enemySnap = visible ? visibleColor : snaplineColor;

			const FLinearColor boxC = rgb ? rgbColor : (friendly ? friendColor : enemyBox);
			const FLinearColor bonesC = rgb ? rgbColor : (friendly ? friendColor : enemyBones);
			const FLinearColor snapC = rgb ? rgbColor : (friendly ? friendColor : enemySnap);
			const FLinearColor nameC = friendly ? friendColor : nameColor;

			auto Mesh = Character->Mesh;

			FVector2D head = Projection::Bone(Mesh, BoneFNames::head);
			FVector2D feet = Projection::Bone(Mesh, BoneFNames::Root);
			if (OffScreen(feet)) continue;

			if (visuals.Snaplines)
				Render::Line({Globals::Canvas->ClipX * 0.5f, Globals::Canvas->ClipY}, feet, 1.f, snapC);

			if (visuals.Box3D)
				DrawBox3D(controller, cached.location, boxC);
			else if (visuals.Box && !OffScreen(head))
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
					Render::Text(feet, state->PlayerNamePrivate.ToString(), visuals.FontScale, nameC);
			}

			// Sub-labels stack downward below the name (which sits at `feet`).
			float ty = feet.Y + 14.f;

			if (visuals.Distance && hasPlayer)
			{
				Render::Text({feet.X, ty}, std::to_string((int)dist) + "m", visuals.FontScale, nameC);
				ty += 14.f;
			}

			if (visuals.KD)
			{
				std::string text = std::to_string(cached.kills) + "/" + std::to_string(cached.deaths);
				if (cached.killstreak > 0) text += " [" + std::to_string(cached.killstreak) + "]";
				Render::Text({feet.X, ty}, text, visuals.FontScale, nameC);
				ty += 14.f;
			}

			if (visuals.BotTag && cached.isBot)
			{
				Render::Text({feet.X, ty}, "BOT", visuals.FontScale, nameC);
				ty += 14.f;
			}
		}
	};
};
