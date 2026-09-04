#pragma once

/// @file
/// @brief Named settings profiles and portable "share codes".
///
/// Profiles are full SETTINGS documents saved under `settings/profiles/<name>.settings` via the
/// shared SettingsFile<SETTINGS>. A share code is the current SETTINGS serialized to JSON and
/// base64-encoded, so a config can be pasted between users. Loading/importing replaces the live
/// Settings and dispatches SettingsChanged (so features refresh and autosave runs).

#include <filesystem>
#include <string>
#include <vector>

#include "Settings.h"
#include "../utils/Base64.h"
#include "../scripting/Events.h"

namespace Profiles
{
	namespace fs = std::filesystem;

	/// The profiles directory (created on demand).
	inline fs::path Dir()
	{
		auto dir = Shared::AppDataPath(SettingsHelper::AppFolder) / "profiles";
		std::error_code ec;
		fs::create_directories(dir, ec);
		return dir;
	}

	/// On-disk path for a named profile.
	inline fs::path PathFor(const std::string& name)
	{
		return Dir() / (name + ".settings");
	}

	/// Names of the saved profiles (the `.settings` files under the profiles dir, without extension).
	inline std::vector<std::string> List()
	{
		std::vector<std::string> names;
		std::error_code ec;
		for (const auto& entry : fs::directory_iterator(Dir(), ec))
		{
			if (entry.path().extension() == ".settings") names.push_back(entry.path().stem().string());
		}
		return names;
	}

	/// Save the current settings as a named profile.
	inline void Save(const std::string& name)
	{
		Shared::SettingsFile<SETTINGS> file(Settings, PathFor(name));
		file.Save();
	}

	/// Load a named profile into the live settings and dispatch SettingsChanged. False if missing.
	inline bool Load(const std::string& name)
	{
		Shared::SettingsFile<SETTINGS> file(Settings, PathFor(name));
		if (!file.Load()) return false;

		Events::Dispatch(Events::Type::SettingsChanged);
		return true;
	}

	/// Delete a named profile from disk.
	inline void Delete(const std::string& name)
	{
		std::error_code ec;
		fs::remove(PathFor(name), ec);
	}

	/// A portable share code for the current settings (base64 of the settings JSON).
	inline std::string Export()
	{
		return Base64::Encode(json(Settings).dump());
	}

	/// Apply a share code to the live settings (tolerant, like a normal load). False on bad input.
	inline bool Import(const std::string& code)
	{
		try
		{
			Settings = json::parse(Base64::Decode(code)).get<SETTINGS>();
			Events::Dispatch(Events::Type::SettingsChanged);
			return true;
		}
		catch (const std::exception&)
		{
			return false;
		}
	}
} // namespace Profiles
