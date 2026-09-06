#pragma once

/// @file
/// The AnnounceToggles feature: when a tracked feature is toggled, show a **client-only** chat line
/// by calling ClientUpdateChat locally (the client-receive RPC — it only paints the message in *your*
/// chat box, nothing is sent to the server or other players), labeled e.g. "[ESP] Enabled". Runs on
/// the SettingsChanged event and diffs a snapshot of the tracked bools, so it reports what changed.

#include "Feature.h"
#include "../ue/Engine.h"

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

	static constexpr int Count = 10;
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
			{"Infinite Ammo", &Settings.EXPLOITS.InfiniteAmmo},
			{"No Reload", &Settings.EXPLOITS.NoReload},
		}};
		return watched;
	}

  public:
	AnnounceToggles()
	{
		Name = "AnnounceToggles";
		Triggers = {Events::Type::SettingsChanged}; // driven by menu changes, not per-frame
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
		const bool inGame = Engine::IsInGame;

		for (int i = 0; i < Count; i++)
		{
			const bool current = *watched[i].value;

			// Announce only real changes, only once we have a baseline, and only in-game (a
			// client message needs a valid controller). The snapshot still updates otherwise.
			if (snapshotReady && current != previous[i] && inGame)
			{
				std::string message = std::format("[{}] {}", watched[i].label, current ? "Enabled" : "Disabled");

				// Client-only: ClientUpdateChat is the server->client receive RPC; invoking it on our
				// own controller just displays the line locally (never leaves this client).
				FTextChatData data{};
				data.SenderName = FString(std::string("[Splitgate]"));
				data.SenderText = FString(message);
				data.NiceText = FString(message);
				data.ChatType = EChatType::General;
				Engine::PlayerController->ClientUpdateChat(data);
			}

			previous[i] = current;
		}

		snapshotReady = true;
	};
};
