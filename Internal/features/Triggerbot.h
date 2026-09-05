#pragma once

/// @file
/// The Triggerbot feature: while the trigger key is held, fires (a synthetic left click) once the
/// crosshair is on an enemy, after a configurable delay. Reuses the shared ActorCache.
///
/// NOTE: "on target" is approximated by a body-bone being within TriggerFov pixels of the crosshair
/// (not a real trace), so the FOV/delay want in-game tuning.

#include "Feature.h"
#include "../utils/Globals.h"
#include "../cache/ActorCache.h"
#include "../utils/WorldToScreen.h"
#include "../utils/Visibility.h"

#include <Windows.h>
#include <chrono>
#include <cmath>

class Triggerbot : public Feature
{
  private:
	std::chrono::steady_clock::time_point acquiredAt{}; ///< when the crosshair first landed on a target
	bool onTarget = false;								///< currently on a target (edge tracking for the delay)

	/// Send a single synthetic left mouse click to the focused window (the game).
	static void Fire()
	{
		INPUT input[2] = {};
		input[0].type = INPUT_MOUSE;
		input[0].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
		input[1].type = INPUT_MOUSE;
		input[1].mi.dwFlags = MOUSEEVENTF_LEFTUP;
		SendInput(2, input, sizeof(INPUT));
	}

  public:
	Triggerbot()
	{
		Name = "Triggerbot";
		UpdateEnabled();

		Log("Created");
	};

	void UpdateEnabled()
	{
		Enabled = Settings.AIM.Triggerbot;
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
		Initialized = true;
		Log("Initialized");
	};

	void Destroy() {
	};

	void Run()
	{
		const auto& aim = Settings.AIM;
		if (!(GetAsyncKeyState(aim.TriggerKey) & 0x8000))
		{
			onTarget = false;
			return;
		}

		auto* controller = Globals::PlayerController;
		auto* localPawn = controller->AcknowledgedPawn;
		if (!localPawn) return;

		char localTeam = -1;
		if (auto* localChar = reinterpret_cast<APortalWarsCharacter*>(controller->Character))
			localTeam = localChar->GetTeamNum();

		const FVector2D crosshair{Globals::Canvas->ClipX * 0.5f, Globals::Canvas->ClipY * 0.5f};

		bool nowOnTarget = false;
		for (const auto& cached : ActorCache::Players())
		{
			auto* character = cached.character;
			if (reinterpret_cast<AActor*>(character) == reinterpret_cast<AActor*>(localPawn)) continue;
			if (ActorCache::IsDead(cached)) continue; // don't fire at a dead body
			if (aim.TriggerTeamCheck && localTeam >= 0 && cached.team == localTeam) continue;
			if (aim.AimVisibleCheck && !Visibility::IsVisible(character, 0.1f)) continue; // only visible targets

			auto* mesh = character->Mesh;
			if (!mesh) continue;

			for (const int bone : {(int)BoneFNames::head, (int)BoneFNames::spine_03, (int)BoneFNames::pelvis})
			{
				const FVector2D screen = Projection::Bone(mesh, bone);
				if (!screen.X && !screen.Y) continue;

				const float dx = screen.X - crosshair.X, dy = screen.Y - crosshair.Y;
				if (sqrtf(dx * dx + dy * dy) <= aim.TriggerFov)
				{
					nowOnTarget = true;
					break;
				}
			}
			if (nowOnTarget) break;
		}

		const auto now = std::chrono::steady_clock::now();
		if (!nowOnTarget)
		{
			onTarget = false;
			return;
		}

		if (!onTarget)
		{
			onTarget = true;
			acquiredAt = now;
		}

		if (std::chrono::duration_cast<std::chrono::milliseconds>(now - acquiredAt).count() >= aim.TriggerDelay)
		{
			Fire();
			acquiredAt = now; // re-arm the delay before the next shot
		}
	};
};
