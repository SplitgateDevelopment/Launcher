#pragma once

#include "../external/discord-rpc/include/discord_rpc.h"
#include "../utils/Logger.h"

DiscordEventHandlers DiscordRPCHandlers;

namespace DiscordHandlers {
	void OnError(int errorCode, const char* message) {
		Logger::Log("ERROR", message);
	};


	void OnReady(const DiscordUser* request) {
		Logger::Log("SUCCESS", std::format("RPC ready for user {}", request->username));
	};

	void OnDisconnected(int errorCode, const char* message) {
		Logger::Log("ERROR", message);
	};

	void Init() {
		memset(&DiscordRPCHandlers, 0, sizeof(DiscordRPCHandlers));

		DiscordRPCHandlers.ready = OnReady;
		DiscordRPCHandlers.errored = OnError;
		DiscordRPCHandlers.disconnected = OnDisconnected;

		Logger::Log("RPC", "Loaded handlers");
	};
};