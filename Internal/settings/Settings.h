#pragma once

#include <filesystem>

namespace fs = std::filesystem;

//don't use std::string

typedef struct {
	struct {
		bool ShowMenu;
		bool ShowWatermark;
		const char* Watermark;
		int ShowHotkey;
	} MENU;

	struct {
		float FOV;
		bool GodMode;
		bool SpinBot;
		int NoClip;
		bool NoRecoil;
		bool GodMelee;
		float PlayerSpeed;
		bool InfinteJetpack;
		bool InfiniteAmmo;
		bool NoReload;
	} EXPLOITS;

	struct {
		bool LoadIntoMap;
		bool ShowConsole;
		const char* PlayerName;
		const char* DiscordAppID;
		bool UserScriptsEnabled;
	} MISC;

	struct {
		bool LogProcessEvent;
		bool FeaturesLogging;
		bool ShowDemoWindow;
		bool ShowStyleEditor;
		bool DrawActors;
	} DEBUG;
} SETTINGS;

extern SETTINGS Settings;

namespace SettingsHelper {
	fs::path GetAppPath();
	std::string GetSettingsFilePath();
	bool Init();
	VOID Save();
	VOID Reset();
	VOID Delete();
}