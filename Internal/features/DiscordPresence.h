#pragma once

/// @file
/// The DiscordPresence feature: refreshes the Discord Rich Presence with live game state (map +
/// K/D) on a throttle, so the presence stays current without spamming Discord every frame.

#include "Feature.h"
#include "../discord/rpc.h"

#include <chrono>

class DiscordPresence : public Feature
{
  private:
	std::chrono::steady_clock::time_point lastUpdate{};
	static constexpr int IntervalSeconds = 5;

  public:
	DiscordPresence()
	{
		Name = "DiscordPresence";
		UpdateEnabled();
		Log("Created");
	};

	void UpdateEnabled()
	{
		Enabled = Settings.MISC.DiscordRPCEnabled;
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

	void Destroy() {
	};

	void Run()
	{
		const auto now = std::chrono::steady_clock::now();
		if (std::chrono::duration_cast<std::chrono::seconds>(now - lastUpdate).count() < IntervalSeconds) return;

		lastUpdate = now;
		DiscordRPC::UpdateGameState();
	};
};
