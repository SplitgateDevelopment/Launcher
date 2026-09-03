#pragma once

#include "../../shared/Settings.h"
#include "../../Internal/settings/Settings.h" // NetworkSettings, ProxyMode

/**
 * @file
 * @brief Launcher-side read of the DLL's NETWORK settings. The proxy config is the single
 * source of truth in the DLL's settings file; the launcher loads only that section (a narrow
 * view — nlohmann ignores the game-only sections) to decide whether to spawn mitmproxy.
 */
namespace Launcher
{
	/// Narrow view of the DLL settings file exposing only the NETWORK section.
	struct NetworkView
	{
		NetworkSettings NETWORK;
	};

	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(NetworkView, NETWORK)

	/// Reads NETWORK from the DLL's settings file (Documents\SplitgateInternal\splitgate.settings),
	/// falling back to defaults if it doesn't exist yet.
	inline NetworkSettings ReadNetworkSettings()
	{
		NetworkView view;
		Shared::SettingsFile<NetworkView> file(view, Shared::AppDataPath("SplitgateInternal", "splitgate.settings"));
		file.Load();
		return view.NETWORK;
	}
} // namespace Launcher
