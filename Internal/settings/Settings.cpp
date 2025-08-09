#include "Settings.h"


SETTINGS Settings = SETTINGS{};

namespace SettingsHelper {
	fs::path folder("SplitgateInternal");
	fs::path filename("splitgate.settings");

	fs::path GetAppPath(std::string filename) {
		wchar_t* documentsPath = nullptr;

		HRESULT result = SHGetKnownFolderPath(FOLDERID_Documents, 0, NULL, &documentsPath);
		if (!SUCCEEDED(result)) return "";

		std::wstring documentsPathStr(documentsPath);
		CoTaskMemFree(documentsPath);

		fs::path appPath = fs::path(documentsPathStr) / folder;
		fs::create_directories(appPath);

		return appPath / filename;
	}

	std::string GetSettingsFilePath() {
		return GetAppPath(filename.string()).string();
	}

	void Save() {
		std::string path = GetSettingsFilePath();
		std::ofstream file(path, std::ios::out | std::ios::binary);

		if (!file.is_open() || !file.good()) {
			char errorMsg[256];
			strerror_s(errorMsg, sizeof(errorMsg), errno);

			std::cerr << "[SettingsHelper] Failed to open settings file for writing: " << errorMsg << std::endl;

			file.close();
			return;
		}

		try {
			json j = Settings;
			file << j.dump(4);
		}
		catch (const std::exception& e) {
			std::cerr << "[SettingsHelper] Failed to save settings: " << e.what() << "\n";
		}

		file.close();
	}

	bool Load() {
		std::string path = GetSettingsFilePath();
		std::ifstream file(path, std::ios::in | std::ios::binary);

		if (!fs::exists(path)) return false;
		if (!file.is_open()) return false;
		if (!file.good()) return false;

		try {
			json j;
			file >> j;

			Settings = j.get<SETTINGS>();
		}
		catch (const std::exception& e) {
			std::cerr << "[SettingsHelper] Failed to load settings: " << e.what() << "\n";
			return false;
		}

		file.close();
		return true;
	}

	void Reset() {
		Settings = SETTINGS{};
	}

	void Delete() {
		std::string path = GetSettingsFilePath();
		if (!fs::exists(path)) return;

		fs::remove(path);
	}
}