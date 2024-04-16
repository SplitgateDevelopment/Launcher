#include <Windows.h>
#include <iostream>
#include "Settings.h"
#include <ShlObj.h>
#include <fstream>

SETTINGS Settings;

namespace SettingsHelper {
	fs::path filename("splitgate.settings");
	fs::path GetAppPath() {
		wchar_t* documentsPath = nullptr;
		std::string appPath;

		HRESULT result = SHGetKnownFolderPath(FOLDERID_Documents, 0, NULL, &documentsPath);
		if (SUCCEEDED(result)) {
			std::wstring documentsPathStr(documentsPath);

			CoTaskMemFree(documentsPath);

			fs::path appPath = fs::path(documentsPathStr) / "SplitgateInternal";
			fs::create_directories(appPath);

			return appPath;
		}

		return appPath;
	}

	std::string GetSettingsFilePath() {
		return (GetAppPath() / filename).string();
	}

	VOID Save() {
		std::string path = GetSettingsFilePath();
		std::ofstream file(path, std::ios::out | std::ios::binary);

		if (!file.is_open()) {
			char errorMsg[256];
			strerror_s(errorMsg, sizeof(errorMsg), errno);
			std::cerr << "Failed to open settings file for writing: " << errorMsg << std::endl;
			return;
		}

		file.write(reinterpret_cast<char*>(&Settings), sizeof(SETTINGS));
		file.close();
	}

	bool Init() {
		std::string path = GetSettingsFilePath();
		std::ifstream file(path, std::ios::in | std::ios::binary);

		if (!file.is_open()) {
			Reset();
			return false;
		}

		file.read(reinterpret_cast<char*>(&Settings), sizeof(SETTINGS));
		file.close();

		if (sizeof(Settings) != sizeof(SETTINGS)) {
			Reset();
			return false;
		}

		return true;
	}

	VOID Reset() {
		Settings = { 0 };
		
		Settings.MENU.ShowMenu = true;
		Settings.MENU.ShowWatermark = true;
		Settings.MENU.Watermark = "github.com/SplitgateDevelopment/Launcher";
		Settings.MENU.ShowHotkey = VK_INSERT;
		Settings.MENU.MenuPosition = { 0.f, 0.f };
		Settings.MENU.MenuColor = { 255.0f, 0.0f, 0.0f, 1.0f };

		Settings.EXPLOITS.FOV = 80;
		Settings.EXPLOITS.GodMode = false;
		Settings.EXPLOITS.SpinBot = false;
		Settings.EXPLOITS.NoClip = VK_OEM_PLUS;

		Settings.MISC.LoadIntoMap = false;
		Settings.MISC.ShowConsole = true;
		Settings.MISC.PlayerName = "SplitgateDevelopment";
		Settings.MISC.DiscordAppID = "1078744504066117703";
		Settings.MISC.UserScriptsEnabled = false;
		Settings.MISC.LogProcessEvent = false;

		Save();
	}

	VOID Delete() {
		std::string path = GetSettingsFilePath();

		if (fs::exists(path)) {
			fs::remove(path);
		};
	}
}