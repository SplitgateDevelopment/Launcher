#pragma once
#include <chrono>
#include "include/discord_rpc.h"
#include "include/discord_register.h"
#include "../utils/Logger.h"

#pragma comment(lib, "discord-rpc.lib")

class DiscordRPC {
	Logger* gLogger;
public:
	void Init(const char* appId, Logger* logger) {
		gLogger = logger;

		DiscordEventHandlers handlers;
		memset(&handlers, 0, sizeof(handlers));
		logger->log("RPC", "Loaded handlers");

		Discord_Initialize(appId, &handlers, 1, "0");
		logger->log("RPC", "Initialized RPC");
	}

	void UpdatePresence() {
		int64_t StartTime = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();

		memset(&discordPresence, 0, sizeof(discordPresence));
		discordPresence.state = "Injected";
		discordPresence.details = "github.com/SplitgateDevelopment/Launcher";
		discordPresence.startTimestamp = StartTime;
		discordPresence.endTimestamp = NULL;
		discordPresence.largeImageKey = "icon";
		discordPresence.largeImageText = "SplitgateInternal";
		discordPresence.instance = 1;
		discordPresence.partySize = 1;
		discordPresence.partyMax = 1;
		discordPresence.partyId = "aaaa";
		discordPresence.joinSecret = "bbbb";
		discordPresence.spectateSecret = "cccc";
		discordPresence.matchSecret = "dddd";

		Discord_UpdatePresence(&discordPresence);
	};
private:
	DiscordRichPresence discordPresence;
};