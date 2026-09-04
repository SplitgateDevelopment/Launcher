#pragma once

/// @file
/// The AnnounceToggles feature: when a tracked feature is toggled, post a **client-only** chat
/// line (APlayerController::ClientMessage — shown only to you, not sent to the server) labeled by
/// the feature, e.g. "[ESP] Enabled". Runs on the SettingsChanged event and diffs a snapshot of the
/// tracked bools, so it reports exactly what changed.

#include "Feature.h"
#include "../utils/Globals.h"

#include <array>
#include <format>
#include <string>

class AnnounceToggles : public Feature
{
  private:
	/// A tracked toggle: its label and the setting bool to watch.
	struct Tracked
	{
		const char* label;
		bool* value;
	};

	static constexpr int Count = 12;
	std::array<bool, Count> previous{};
	bool snapshotReady = false; ///< false until the first pass seeds `previous` (so we don't announce on enable)

	static const std::array<Tracked, Count>& Watched()
	{
		static const std::array<Tracked, Count> watched = {{
			{"ESP", &Settings.VISUALS.Esp},
			{"Radar", &Settings.VISUALS.Radar},
			{"Aimbot", &Settings.AIM.Aimbot},
			{"Triggerbot", &Settings.AIM.Triggerbot},
			{"God Mode", &Settings.EXPLOITS.GodMode},
			{"Infinite Jetpack", &Settings.EXPLOITS.InfinteJetpack},
			{"No Recoil", &Settings.EXPLOITS.NoRecoil},
			{"Spin Bot", &Settings.EXPLOITS.SpinBot},
			{"Third Person", &Settings.EXPLOITS.ThirdPerson},
			{"Free Cam", &Settings.EXPLOITS.FreeCam},
			{"Infinite Ammo", &Settings.EXPLOITS.InfiniteAmmo},
			{"No Reload", &Settings.EXPLOITS.NoReload},
		}};
		return watched;
	}

  public:
	AnnounceToggles()
	{
		Name = "AnnounceToggles";
		Event = Events::Type::SettingsChanged; // driven by menu changes, not per-frame
		UpdateEnabled();
		Log("Created");
	};

	void UpdateEnabled()
	{
		Enabled = Settings.MISC.AnnounceToggles;
	};

	bool Check()
	{
		return Initialized;
	};

	void Init()
	{
		Initialized = true;
		Log("Initialized");
	};

	/// Re-seed on the enabled -> disabled edge so re-enabling starts fresh (no stale diffs).
	void Destroy()
	{
		snapshotReady = false;
	};

	void Run()
	{
		const auto& watched = Watched();
		const bool inGame = Globals::PlayerController && Globals::PlayerController->IsInGame();

		for (int i = 0; i < Count; i++)
		{
			const bool current = *watched[i].value;

			// Announce only real changes, only once we have a baseline, and only in-game (a
			// client message needs a valid controller). The snapshot still updates otherwise.
			if (snapshotReady && current != previous[i] && inGame)
			{
				std::string message = std::format("[{}] {}", watched[i].label, current ? "Enabled" : "Disabled");
				Globals::PlayerController->ClientMessage(FString(message), FName{}, 4.f);
			}

			previous[i] = current;
		}

		snapshotReady = true;
	};
};
