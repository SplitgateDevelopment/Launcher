#pragma once

/// @file
/// The DiscordPresence feature: refreshes the Discord Rich Presence with live game state (map +
/// K/D). Event-driven rather than per-frame — it runs on the in-game/in-lobby transitions
/// (EnteredGame / EnteredLobby) and on each kill (PlayerKilled) so the K/D stays current.
/// UpdateGameState only pushes to Discord when the string actually changes, so PlayerKilled
/// (which fires for every kill in the match) effectively updates only on the local player's
/// own kills/deaths.

#include "Feature.h"
#include "../discord/rpc.h"

class DiscordPresence : public Feature
{
  public:
	DiscordPresence()
	{
		Name = "DiscordPresence";
		Triggers = {Events::Type::EnteredGame, Events::Type::EnteredLobby, Events::Type::PlayerKilled};
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

	/// Push the current game state to Discord on the event that fired us (a state transition or a
	/// kill). UpdateGameState no-ops when the resulting presence string is unchanged.
	void Run() override
	{
		DiscordRPC::UpdateGameState();
	};
};
