#pragma once

#include "../../shared/Settings.h"
#include "../../shared/LauncherSettings.h"	  // Shared::LauncherSettings
#include "../../Internal/settings/Settings.h" // NetworkSettings, ProxyMode

/**
 * @file
 * @brief Launcher-side reads of settings. Two files back the proxy:
 * - the DLL's `splitgate.settings` NETWORK section (the DLL owns it) tells the launcher whether
 *   to spawn mitmproxy and which hosts to redirect;
 * - the launcher's own `launcher.settings` (Shared::LauncherSettings) tells it *how* to spawn
 *   mitmdump. It is kept out of the DLL's SETTINGS so the internal file stays game-focused.
 * Both are loaded through a narrow view — nlohmann ignores keys it doesn't know.
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
		Shared::SettingsFile<NetworkView> file(view, Shared::AppDataPath(SettingsHelper::AppFolder, SettingsHelper::SettingsFileName));
		file.Load();
		return view.NETWORK;
	}

	/// Reads the launcher's own settings (Documents\SplitgateInternal\launcher.settings), falling
	/// back to defaults if the file doesn't exist yet.
	inline Shared::LauncherSettings ReadLauncherSettings()
	{
		Shared::LauncherSettings settings;
		Shared::SettingsFile<Shared::LauncherSettings> file(settings, Shared::AppDataPath(SettingsHelper::AppFolder, Shared::LauncherSettingsFileName));
		file.Load();
		return settings;
	}
} // namespace Launcher
