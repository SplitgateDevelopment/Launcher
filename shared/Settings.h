#pragma once

#include <Windows.h>
#include <ShlObj.h>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

#include <nlohmann/json.hpp>

/**
 * @file
 * @brief Generic JSON settings persistence, reusable by both the DLL and the launcher.
 *
 * `SettingsFile<T>` binds an nlohmann-serializable struct T to a file path and loads/saves it,
 * so each side owns its own settings type (the DLL its full SETTINGS, the launcher a narrow
 * view). `AppDataPath` resolves the shared on-disk location so both can find the same file.
 */
namespace Shared
{
	/// Resolves `<Documents>/<folder>[/<filename>]`, creating the folder. Empty path on failure.
	inline std::filesystem::path AppDataPath(const std::string& folder, const std::string& filename = "")
	{
		wchar_t* documents = nullptr;
		if (!SUCCEEDED(SHGetKnownFolderPath(FOLDERID_Documents, 0, nullptr, &documents))) return {};

		std::filesystem::path base = std::filesystem::path(documents) / folder;
		CoTaskMemFree(documents);

		std::filesystem::path result = filename.empty() ? base : base / filename;

		// Create the directory that will hold the result — the base folder, or a nested folder
		// when filename carries subdirectories (e.g. "logs/internal.log").
		std::error_code ec;
		std::filesystem::create_directories(filename.empty() ? result : result.parent_path(), ec);
		return result;
	}

	/**
	 * A JSON file bound to an external settings struct @p T (which must have nlohmann
	 * from_json/to_json — e.g. via NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT). The path
	 * is caller-supplied, so the same T can be stored anywhere.
	 */
	template <typename T>
	class SettingsFile
	{
	  public:
		/// Binds @p data to the file at @p path. Nothing is read or written until Load/Save.
		/// @param data the settings struct this file loads into / saves from (kept by reference).
		/// @param path the on-disk location.
		SettingsFile(T& data, std::filesystem::path path) : value(data), path(std::move(path)) {}

		/// @return the on-disk path.
		const std::filesystem::path& Path() const { return path; }

		/**
		 * Reads the file into the bound struct, tolerating missing keys (WITH_DEFAULT) and
		 * ignoring unknown ones (so a narrow view can read a wider file).
		 * @return true if a file was read, false if none existed or it couldn't be parsed.
		 */
		bool Load()
		{
			if (!std::filesystem::exists(path)) return false;

			std::ifstream file(path, std::ios::in | std::ios::binary);
			if (!file.is_open() || !file.good()) return false;

			bool ok = true;
			try
			{
				nlohmann::json json;
				file >> json;
				value = json.get<T>();
			}
			catch (const std::exception& e)
			{
				std::cerr << "[Settings] Failed to load " << path.string() << ": " << e.what() << std::endl;
				ok = false;
			}

			// Release the handle before returning so the other process (launcher/DLL) can open
			// the file. (The stream would also close on scope exit, but be explicit.)
			file.close();
			return ok;
		}

		/// Serializes the bound struct to the file (pretty-printed).
		void Save()
		{
			std::ofstream file(path, std::ios::out | std::ios::binary);
			if (!file.is_open() || !file.good())
			{
				std::cerr << "[Settings] Failed to open " << path.string() << " for writing" << std::endl;
				return;
			}

			try
			{
				file << nlohmann::json(value).dump(4);
			}
			catch (const std::exception& e)
			{
				std::cerr << "[Settings] Failed to save " << path.string() << ": " << e.what() << std::endl;
			}

			file.close(); // release the handle before returning (see Load)
		}

		/// Resets the bound struct to defaults in memory (does not save).
		void Reset() { value = T{}; }

		/// Deletes the file from disk if it exists.
		void Remove()
		{
			std::error_code ec;
			std::filesystem::remove(path, ec);
		}

	  private:
		T& value;					///< the bound settings struct (loaded into / saved from)
		std::filesystem::path path; ///< the file location
	};
} // namespace Shared
