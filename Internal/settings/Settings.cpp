/**
 * @file
 * @brief Defines the global @ref Settings instance and the settings-file accessor bound to it.
 * Persistence and app-path resolution live in shared/Settings.h (Shared::SettingsFile /
 * Shared::AppDataPath), included via Settings.h.
 */

#include "Settings.h"

/// The one global settings instance (declared extern in Settings.h).
SETTINGS Settings = SETTINGS{};

namespace SettingsHelper
{
	Shared::SettingsFile<SETTINGS>& File()
	{
		static Shared::SettingsFile<SETTINGS> instance(Settings, Shared::AppDataPath(AppFolder, SettingsFileName));
		return instance;
	}
} // namespace SettingsHelper
