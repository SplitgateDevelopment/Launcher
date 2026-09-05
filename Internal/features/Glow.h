#pragma once

/// @file
/// The Glow feature: forces a custom-depth outline (chams) on characters so they show through
/// walls, in a per-team color (or the RGB rainbow). Reuses the game's own stencil-outline
/// post-process — see the class comment for the in-game caveats.

#include "Feature.h"
#include "../utils/Globals.h"
#include "../cache/ActorCache.h"
#include "../utils/Rgb.h"

// Chams / glow: for each cached enemy (and optionally teammate) character, turns on the mesh's
// custom-depth rendering and sets its stencil value + outline color, so the game's outline
// post-process draws the player through walls in our color. The color path overwrites the
// character's Blue/RedOutlineColor fields (both, so it wins whichever the post-process samples).
//
// NOTE: this rides on the game's existing team-outline post-process. Whether it maps an arbitrary
// color (vs a fixed per-team one) can only be confirmed in-game; if the stock post-process ignores
// the color, a custom outline material would be needed. The stencil values come from the
// character's own EnemyStencilValue / FriendlyStencilValue so they match what the game expects.
class Glow : public Feature
{
  private:
	/// Convert a settings Color (0-1 RGBA) into the engine FLinearColor.
	static FLinearColor ToColor(const Color& c) { return FLinearColor{c.R, c.G, c.B, c.A}; }

	/// Force the through-wall outline on @p character's mesh with the given stencil value + color.
	static void Apply(APortalWarsCharacter* character, int stencil, const FLinearColor& color)
	{
		auto* mesh = character->Mesh;
		if (!mesh) return;
		mesh->SetRenderCustomDepth(true);
		mesh->CustomDepthStencilValue = stencil;
		character->BlueOutlineColor = color;
		character->RedOutlineColor = color;
	}

  public:
	Glow()
	{
		Name = "Glow";
		UpdateEnabled();

		Log("Created");
	};

	void UpdateEnabled()
	{
		Enabled = Settings.VISUALS.GlowEnemy || Settings.VISUALS.GlowFriendly || Settings.VISUALS.GlowSelf;
	};

	bool Check()
	{
		if (!Initialized) return false;
		if (!Globals::PlayerController) return false;
		if (!Globals::PlayerController->IsInGame()) return false;

		return true;
	};

	void Init()
	{
		Initialized = true;
		Log("Initialized");
	};

	/// Turn our forced outline back off for every character still in the cache; the game restores
	/// its own outlines as usual. Only touches valid, cached meshes (never a despawned pointer).
	void Destroy()
	{
		for (const auto& cached : ActorCache::Players())
			if (auto* mesh = cached.character->Mesh)
				mesh->SetRenderCustomDepth(false);

		if (auto* self = reinterpret_cast<APortalWarsCharacter*>(Globals::PlayerController->Character))
			if (auto* mesh = self->Mesh)
				mesh->SetRenderCustomDepth(false);
	};

	/// Enable the through-wall outline + color for each wanted character. Players that shouldn't
	/// glow are left untouched, so the game keeps managing its own outlines for them.
	void Run()
	{
		const auto& v = Settings.VISUALS;
		const bool rgb = Settings.MENU.Rgb;
		const FLinearColor rgbColor = rgb ? ToColor(Rgb::Current()) : FLinearColor{};

		auto* controller = Globals::PlayerController;
		auto* localPawn = controller->AcknowledgedPawn;

		// Source the stencil values from the local character: they're reliably populated there, whereas
		// a freshly-seen enemy's own Enemy/FriendlyStencilValue can still read 0 (→ stencil 0 = no
		// outline, the "glow doesn't glow" case). These are the values the game's outline post-process
		// already maps to a through-wall color.
		auto* self = reinterpret_cast<APortalWarsCharacter*>(controller->Character);
		if (!self) return;
		const char localTeam = self->GetTeamNum();
		const int enemyStencil = self->EnemyStencilValue;
		const int friendlyStencil = self->FriendlyStencilValue;

		for (const auto& cached : ActorCache::Players())
		{
			auto* character = cached.character;
			if (reinterpret_cast<AActor*>(character) == reinterpret_cast<AActor*>(localPawn)) continue;
			if (ActorCache::IsDead(cached)) continue; // don't glow a dead body on the ground

			const bool friendly = (localTeam >= 0 && cached.team == localTeam);
			if (!(friendly ? v.GlowFriendly : v.GlowEnemy)) continue; // this team's glow is off

			const FLinearColor color = rgb ? rgbColor : ToColor(friendly ? v.GlowFriendlyColor : v.GlowEnemyColor);
			Apply(character, friendly ? friendlyStencil : enemyStencil, color);
		}

		// The local player's own pawn (its 3P mesh — only visible in third person).
		if (v.GlowSelf)
			Apply(self, friendlyStencil, rgb ? rgbColor : ToColor(v.GlowSelfColor));
	};
};
