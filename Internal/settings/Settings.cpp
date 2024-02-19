#include <Windows.h>
#include <iostream>
#include <string>
#include "Settings.h"
#include <errno.h>
#include <stdio.h>

SETTINGS Settings = { 0 };

namespace SettingsHelper {

	std::string GetPath() {
		std::string filename = "splitgate.settings";
		/*
			WCHAR szPath[MAX_PATH];
			GetModuleFileName(NULL, szPath, MAX_PATH);

			std::wstring ws(szPath);
			std::string path(ws.begin(), ws.end());
			
			path += filename;
		*/

		CHAR path[0xFF] = { 0 };
		GetTempPathA(sizeof(path) / sizeof(path[0]), path);

		return std::string(path).append(filename);
	}

	VOID Save() {
		std::string path = GetPath();
		FILE* file;
		errno_t error = fopen_s(&file, path.data(), "wb");

		if (error == 0) {
			fwrite(&Settings, sizeof(Settings), 1, file);
			fclose(file);
			//free(path.data());
		}
	}

	bool Init() {
		std::string path = GetPath();
		FILE* file;
		errno_t error = fopen_s(&file, path.data(), "wb");

		if (error == 0) {
			fseek(file, 0, SEEK_END);
			auto size = ftell(file);

			if (size == sizeof(Settings)) {
				fseek(file, 0, SEEK_SET);
				fread(&Settings, sizeof(Settings), 1, file);
				fclose(file);

				//free(path.data());

				printf("abc");
				return true;
			}

			fclose(file);
		}

		//free(path.data());
		Reset();
		return false;
	}

	VOID Reset() {
		Settings = { 0 };
		
		Settings.MENU.ShowMenu = true;
		Settings.MENU.ShowWatermark = true;
		Settings.MENU.Watermark = "github.com/SplitgateDevelopment/Launcher";
		Settings.MENU.ShowHotkey = VK_INSERT;
		Settings.MENU.MenuPosition = {};

		Settings.EXPLOITS.FOV = 80;
		Settings.EXPLOITS.GodMode = false;
		Settings.EXPLOITS.SpinBot = false;
		Settings.EXPLOITS.NoClip = VK_OEM_PLUS;

		Settings.MISC.LoadIntoMap = false;
		Settings.MISC.Unload = false;
		Settings.MISC.DestroyConsole = false;
		Settings.MISC.PlayerName = "SplitgateDevelopement";
		Settings.MISC.DiscordAppID = "1078744504066117703";

		Save();
	}
}