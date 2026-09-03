/**
 * @file
 * @brief Defines the global @ref Settings instance and the SettingsHelper facade. Persistence
 * is delegated to Shared::SettingsFile (shared with the launcher); the Documents app-path
 * resolution stays here since it's DLL-specific.
 */

#include "Settings.h"
#include "../../shared/Settings.h"

/// The one global settings instance (declared extern in Settings.h).
SETTINGS Settings = SETTINGS{};

namespace SettingsHelper
{
	fs::path folder("SplitgateInternal");
	fs::path filename("splitgate.settings");

	fs::path GetAppPath(std::string filename)
	{
		return Shared::AppDataPath(folder.string(), filename);
	}

	std::string GetSettingsFilePath()
	{
		return GetAppPath(filename.string()).string();
	}

	/// The settings file, bound to the global @ref Settings.
	static Shared::SettingsFile<SETTINGS>& file()
	{
		static Shared::SettingsFile<SETTINGS> instance(Settings, GetSettingsFilePath());
		return instance;
	}

	bool Load()
	{
		return file().Load();
	}
	void Save()
	{
		file().Save();
	}
	void Reset()
	{
		file().Reset();
	}
	void Delete()
	{
		file().Remove();
	}
} // namespace SettingsHelper
