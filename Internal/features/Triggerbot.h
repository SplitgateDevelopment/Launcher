#pragma once

/// @file
/// The Triggerbot feature: while the trigger key is held, fires (a synthetic left click) once the
/// crosshair is on an enemy, after a configurable delay. Reuses the shared ActorCache.
///
/// NOTE: "on target" is approximated by a body-bone being within TriggerFov pixels of the crosshair
/// (not a real trace), so the FOV/delay want in-game tuning.

#include "Feature.h"
#include "../ue/Engine.h"
#include "../cache/ActorCache.h"
#include "../native/WorldToScreen.h"
#include "../native/Visibility.h"
#include "../utils/Input.h"

#include <Windows.h>
#include <chrono>
#include <cmath>

class Triggerbot : public Feature
{
  private:
	std::chrono::steady_clock::time_point acquiredAt{}; ///< when the crosshair first landed on a target
	std::chrono::steady_clock::time_point fireStart{};	///< when the current synthetic click was pressed
	bool onTarget = false;								///< currently on a target (edge tracking for the delay)
	bool firing = false;								///< a synthetic left-button press is currently held

	/// How long to hold the synthetic left button down before releasing it. A same-frame down+up is
	/// often too fast for the game to register as a shot; a short hold makes it a real click.
	static constexpr int HoldMs = 30;

	static void MouseButton(DWORD flag)
	{
		INPUT input{};
		input.type = INPUT_MOUSE;
		input.mi.dwFlags = flag;
		SendInput(1, &input, sizeof(INPUT));
	}
	void Press()
	{
		MouseButton(MOUSEEVENTF_LEFTDOWN);
		firing = true;
		fireStart = std::chrono::steady_clock::now();
	}
	void Release()
	{
		if (!firing) return;
		MouseButton(MOUSEEVENTF_LEFTUP);
		firing = false;
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
		if (!Engine::PlayerController) return false;
		if (!Engine::IsInGame) return false;
		if (!Engine::Canvas) return false;

		return true;
	};

	void Init()
	{
		Initialized = true;
		Log("Initialized");
	};

	void Destroy()
	{
		Release(); // don't leave the button stuck down if the feature is disabled mid-click
	};

	void Run()
	{
		const auto& aim = Settings.AIM;
		if (!Input::Down(aim.TriggerKey))
		{
			Release();
			onTarget = false;
			return;
		}

		auto* controller = Engine::PlayerController;
		auto* localPawn = controller->AcknowledgedPawn;
		if (!localPawn) return;

		char localTeam = -1;
		if (auto* localChar = reinterpret_cast<APortalWarsCharacter*>(controller->Character))
			localTeam = localChar->GetTeamNum();

		const FVector2D crosshair{Engine::Canvas->ClipX * 0.5f, Engine::Canvas->ClipY * 0.5f};

		// Eye origin for the visibility line-of-sight trace.
		FVector eye = localPawn->K2_GetActorLocation();
		eye.Z += 80.f;

		bool nowOnTarget = false;
		for (const auto& cached : ActorCache::Players())
		{
			auto* character = cached.character;
			if (reinterpret_cast<AActor*>(character) == reinterpret_cast<AActor*>(localPawn)) continue;
			if (ActorCache::IsDead(cached)) continue; // don't fire at a dead body
			if (aim.TriggerTeamCheck && localTeam >= 0 && cached.team == localTeam) continue;
			if (aim.IgnoreBots && cached.isBot) continue; // fire only at real players
			if (aim.AimVisibleCheck && !Visibility::IsVisible(character, eye)) continue; // only visible targets

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

		// Release a held click after the hold window (so it reads as a real, separate shot).
		if (firing && std::chrono::duration_cast<std::chrono::milliseconds>(now - fireStart).count() >= HoldMs)
			Release();

		if (!nowOnTarget)
		{
			Release();
			onTarget = false;
			return;
		}

		if (!onTarget)
		{
			onTarget = true;
			acquiredAt = now;
		}

		// Press after the delay, once the previous click has been released (edge-triggered).
		if (!firing && std::chrono::duration_cast<std::chrono::milliseconds>(now - acquiredAt).count() >= aim.TriggerDelay)
		{
			Press();
			acquiredAt = now; // re-arm the delay before the next shot
		}
	};
};
