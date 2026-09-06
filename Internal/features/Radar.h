#pragma once

/// @file
/// The Radar feature: a small 2D radar in the top-right corner plotting enemy
/// players relative to the local player's position and facing. See the class
/// comment for the in-game tuning caveats.

#include "Feature.h"
#include "../ue/Globals.h"
#include "../utils/Rgb.h"
#include "../cache/ActorCache.h"

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
	UObject* CharacterClass = 0; ///< resolved PortalWarsCharacter UClass, used as the actor filter

	static constexpr float Size = 200.f;   // radar panel size, px
	static constexpr float Margin = 20.f;  // distance from the screen edge, px
	static constexpr float Range = 6000.f; // world units mapped to the radar radius

	/// Draw a small plus sign centred at (x, y) with arm length @p half.
	void Cross(float x, float y, float half, float thickness, const FLinearColor& color)
	{
		Render::Line({x - half, y}, {x + half, y}, thickness, color);
		Render::Line({x, y - half}, {x, y + half}, thickness, color);
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
		CharacterClass = Engine::ObjObjects->FindObject("Class PortalWars.PortalWarsCharacter");

		Initialized = (!!CharacterClass);
		Log("Initialized");
	};

	void Destroy() {
	};

	/// Draw the radar panel and player marker, then plot each enemy: rotate its
	/// world delta into player-relative forward/right, scale by Range onto the
	/// radar radius (forward = up), and drop points outside the panel.
	void Run()
	{
		auto* controller = Globals::PlayerController;
		auto* localPawn = controller->AcknowledgedPawn;
		if (!localPawn) return;

		const bool showFriendly = Settings.VISUALS.RadarShowFriendly;
		const FLinearColor friendColor{Settings.VISUALS.FriendColor.R, Settings.VISUALS.FriendColor.G, Settings.VISUALS.FriendColor.B, Settings.VISUALS.FriendColor.A};

		// The local player's team, so teammates plot in FriendColor (and only when shown).
		char localTeam = -1;
		if (auto* localChar = reinterpret_cast<APortalWarsCharacter*>(controller->Character))
			localTeam = localChar->GetTeamNum();

		const FVector playerPos = localPawn->K2_GetActorLocation();
		const float yaw = controller->ControlRotation.Yaw * (3.14159265f / 180.f);
		const float cosYaw = cosf(yaw);
		const float sinYaw = sinf(yaw);

		const float radius = Size * 0.5f;
		const float cx = Globals::Canvas->ClipX - Margin - radius;
		const float cy = Margin + radius;

		// Panel border + own marker: the cycling RGB color when enabled, else the default white.
		const Color self = Settings.MENU.Rgb ? Rgb::Current() : Color{1.f, 1.f, 1.f, 1.f};
		const FLinearColor border{self.R, self.G, self.B, self.A};
		const FLinearColor dot{1.f, 0.f, 0.f, 1.f};

		// Panel border + player marker at the centre.
		const FVector2D tl{cx - radius, cy - radius}, tr{cx + radius, cy - radius};
		const FVector2D bl{cx - radius, cy + radius}, br{cx + radius, cy + radius};
		Render::Line(tl, tr, 1.f, border);
		Render::Line(tr, br, 1.f, border);
		Render::Line(br, bl, 1.f, border);
		Render::Line(bl, tl, 1.f, border);
		Cross(cx, cy, 3.f, 1.f, border);

		// Shared per-frame actor pass (ActorCache); location + team already resolved there.
		for (const auto& cached : ActorCache::Players())
		{
			if (reinterpret_cast<AActor*>(cached.character) == reinterpret_cast<AActor*>(localPawn)) continue;

			const char team = cached.team;
			const bool friendly = (localTeam >= 0 && team == localTeam);
			if (friendly && !showFriendly) continue;
			const FLinearColor color = friendly ? friendColor : dot;

			const FVector enemyPos = cached.location;
			const float dx = enemyPos.X - playerPos.X;
			const float dy = enemyPos.Y - playerPos.Y;

			// Rotate the world delta into player-relative space (forward/right).
			const float forward = dx * cosYaw + dy * sinYaw;
			const float right = -dx * sinYaw + dy * cosYaw;

			const float px = cx + (right / Range) * radius;
			const float py = cy - (forward / Range) * radius; // forward = up

			if (px < cx - radius || px > cx + radius || py < cy - radius || py > cy + radius) continue;

			Cross(px, py, 2.f, 2.f, color);
		}
	};
};
