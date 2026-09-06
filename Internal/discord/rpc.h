#pragma once

/// @file
/// @brief Discord Rich Presence wrapper built on the discord-rpc library.
///
/// Wraps the vendored discord-rpc (Internal/external/discord-rpc) to init,
/// update and tear down the game's rich presence (state, watermark details,
/// timestamps, party fields). Keeps the upstream discord-rpc naming for the
/// library calls it forwards to.
#include <chrono>
#include <format>
#include <string>
#include "../external/discord-rpc/include/discord_rpc.h"
#include "../external/discord-rpc/include/discord_register.h"
#include "../utils/Logger.h"
#include "../ue/Engine.h"
#include "handlers.h"

#pragma comment(lib, "discord-rpc.lib")

namespace DiscordRPC
{
	DiscordRichPresence discordPresence;
	int64_t StartTime = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();

	void Destroy()
	{
		Discord_Shutdown();
		Logger::Log("RPC", "Shut down RPC");
	};

	void InitPresence()
	{
		if (!Settings.MISC.DiscordRPCEnabled) return;

		memset(&discordPresence, 0, sizeof(discordPresence));
		discordPresence.state = "Injected";
		discordPresence.details = Settings.MENU.Watermark.c_str();
		discordPresence.startTimestamp = StartTime;
		discordPresence.endTimestamp = NULL;
		discordPresence.largeImageKey = "icon";
		discordPresence.largeImageText = Settings.MENU.Watermark.c_str();
		discordPresence.instance = 1;
		discordPresence.partySize = 1;
		discordPresence.partyMax = 1;
		discordPresence.partyId = "aaaa";
		discordPresence.joinSecret = "bbbb";
		discordPresence.spectateSecret = "cccc";
		discordPresence.matchSecret = "dddd";

		Discord_UpdatePresence(&discordPresence);
		Logger::Log("RPC", "Init presence");
	};

	void Init()
	{
		DiscordHandlers::Init();
		Discord_Initialize(Settings.MISC.DiscordAppID.c_str(), &DiscordRPCHandlers, 1, Settings.MISC.SteamAppId.c_str());

		Logger::Log("RPC", "Initialized RPC");
		InitPresence();
	}

	const char* GetState()
	{
		if (!Settings.MISC.DiscordRPCEnabled) return "";
		return discordPresence.state;
	}

	void UpdateState(const char* state)
	{
		if (!Settings.MISC.DiscordRPCEnabled) return;
		if (GetState() == state) return;

		Logger::Log("RPC", std::format("Updating presence state [{}->{}]", discordPresence.state, state));

		discordPresence.state = state;
		Discord_UpdatePresence(&discordPresence);
	}

	void UpdatePresence()
	{
		if (!Settings.MISC.DiscordRPCEnabled) return;

		Discord_UpdatePresence(&discordPresence);
		Logger::Log("RPC", "Updated presence");
	};

	/// Refresh the presence `state` from the live game: in a match it shows the map and the local
	/// player's K/D (read from APortalWarsPlayerState.PlayerStats), otherwise "In menu". Cheap field
	/// reads, no ProcessEvent; call throttled from the game thread (the DiscordPresence feature).
	/// Only pushes to Discord when the string actually changes.
	void UpdateGameState()
	{
		if (!Settings.MISC.DiscordRPCEnabled) return;

		static std::string stateBuffer; // persists so discordPresence.state stays valid across updates

		auto* controller = Engine::PlayerController;
		if (controller && controller->IsInGame())
		{
			const std::string map = Engine::World ? Engine::World->GetName() : "";
			int kills = 0, deaths = 0;
			if (auto* state = reinterpret_cast<APortalWarsPlayerState*>(controller->PlayerState))
			{
				kills = state->PlayerStats.Kills;
				deaths = state->PlayerStats.Deaths;
			}
			stateBuffer = std::format("In match: {}  (K/D {}/{})", map, kills, deaths);
		}
		else
		{
			stateBuffer = "In menu";
		}

		if (discordPresence.state && stateBuffer == discordPresence.state) return; // unchanged

		discordPresence.state = stateBuffer.c_str();
		Discord_UpdatePresence(&discordPresence);
	}
}; // namespace DiscordRPC