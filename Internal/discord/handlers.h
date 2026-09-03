#pragma once

/// @file
/// @brief discord-rpc callback handlers (ready / errored / disconnected).
///
/// Fills the DiscordEventHandlers struct passed to Discord_Initialize with
/// logging callbacks for the connection lifecycle. Keeps the upstream
/// discord-rpc naming for the callback signatures.
#include "../external/discord-rpc/include/discord_rpc.h"
#include "../utils/Logger.h"

DiscordEventHandlers DiscordRPCHandlers;

namespace DiscordHandlers
{
	void OnError(int errorCode, const char* message)
	{
		Logger::Log("ERROR", message);
	};

	void OnReady(const DiscordUser* request)
	{
		Logger::Log("SUCCESS", std::format("RPC ready for user {}", request->username));
	};

	void OnDisconnected(int errorCode, const char* message)
	{
		Logger::Log("ERROR", message);
	};

	void Init()
	{
		memset(&DiscordRPCHandlers, 0, sizeof(DiscordRPCHandlers));

		DiscordRPCHandlers.ready = OnReady;
		DiscordRPCHandlers.errored = OnError;
		DiscordRPCHandlers.disconnected = OnDisconnected;

		Logger::Log("RPC", "Loaded handlers");
	};
}; // namespace DiscordHandlers