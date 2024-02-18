#pragma once
#include <chrono>
#include "../external/discord-rpc/include/discord_rpc.h"
#include "../external/discord-rpc/include/discord_register.h"
#include "../utils/Logger.h"
#include "handlers.h"

#pragma comment(lib, "discord-rpc.lib")

class DiscordRPC {
public:
	~DiscordRPC() {
		Discord_Shutdown();
	};

	void Init(const char* appId) {
		DiscordHandlers::Init();

		Discord_Initialize(appId, &DiscordRPCHandlers, 1, "677620");
		Logger::Log("RPC", "Initialized RPC");
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
		Logger::Log("RPC", "Updated presence");
	};

	void UpdateState(const char* state) {
		discordPresence.state = state;
		Discord_UpdatePresence(&discordPresence);
		Logger::Log("RPC", format("Updated to state [{}]", state));
	}

	const char* GetState() {
		return discordPresence.state;
	}
private:
	DiscordRichPresence discordPresence;
};