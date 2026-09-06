#pragma once

/// @file
/// The DiscordPresence feature: refreshes the Discord Rich Presence with live game state (map +
/// K/D) whenever the local player enters a match or returns to the lobby. Event-driven (EnteredGame
/// / EnteredLobby) rather than per-frame, so it only touches Discord on an actual state change.

#include "Feature.h"
#include "../discord/rpc.h"

class DiscordPresence : public Feature
{
  public:
	DiscordPresence()
	{
		Name = "DiscordPresence";
		Triggers = {Events::Type::EnteredGame, Events::Type::EnteredLobby};
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

	/// Push the current game state to Discord on the in-game / in-lobby transition that fired us.
	void Run(Events::Type event) override
	{
		Log(event == Events::Type::EnteredGame ? "entered game" : "entered lobby");
		DiscordRPC::UpdateGameState();
	};
};
