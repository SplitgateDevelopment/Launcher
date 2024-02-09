#pragma once
#include <chrono>
#include "include/discord_rpc.h"
#include "include/discord_register.h"
#include "../utils/Logger.h"

#pragma comment(lib, "discord-rpc.lib")

class DiscordRPC {
public:
	Logger* gLogger;

	DiscordRPC(Logger* logger)
	{
		gLogger = logger;
	};

	void Init(const char* appId) {

		DiscordEventHandlers handlers;
		memset(&handlers, 0, sizeof(handlers));
		gLogger->log("RPC", "Loaded handlers");

		Discord_Initialize(appId, &handlers, 1, "677620");
		gLogger->log("RPC", "Initialized RPC");
	}

	void UpdatePresence() {
		int64_t StartTime = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();

		memset(&discordPresence, 0, sizeof(discordPresence));
		discordPresence.state = "Injected";
		discordPresence.details = Settings.MENU.Watermark.data();
		discordPresence.startTimestamp = StartTime;
		discordPresence.endTimestamp = NULL;
		discordPresence.largeImageKey = "icon";
		discordPresence.largeImageText = Settings.MENU.Watermark.data();
		discordPresence.instance = 1;
		discordPresence.partySize = 1;
		discordPresence.partyMax = 1;
		discordPresence.partyId = "aaaa";
		discordPresence.joinSecret = "bbbb";
		discordPresence.spectateSecret = "cccc";
		discordPresence.matchSecret = "dddd";

		Discord_UpdatePresence(&discordPresence);
		gLogger->log("RPC", "Updated presence");
	};

	void UpdateState(const char* state) {
		discordPresence.state = state;
		Discord_UpdatePresence(&discordPresence);
		gLogger->log("RPC", format("Updated to state [{}]", state));
	}

	const char* GetState() {
		return discordPresence.state;
	}
private:
	DiscordRichPresence discordPresence;
};