#pragma once
#include <chrono>
#include "../external/discord-rpc/include/discord_rpc.h"
#include "../external/discord-rpc/include/discord_register.h"
#include "../utils/Logger.h"
#include "handlers.h"

#pragma comment(lib, "discord-rpc.lib")

namespace DiscordRPC {
	DiscordRichPresence discordPresence;
	int64_t StartTime = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();

	void Destroy()
	{
		Discord_Shutdown();
		Logger::Log("RPC", "Shut down RPC");
	};

	void InitPresence() {
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

	const char* GetState() {
		if (!Settings.MISC.DiscordRPCEnabled) return "";
		return discordPresence.state;
	}

	void UpdateState(const char* state) {
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
};