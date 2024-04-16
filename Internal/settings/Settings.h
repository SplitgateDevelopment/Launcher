#pragma once

#include "../ue/Engine.h"
#include <filesystem>

namespace fs = std::filesystem;

//don't use std::string

typedef struct {
	struct {
		bool ShowMenu;
		bool ShowWatermark;
		const char* Watermark;
		int ShowHotkey;
		FVector2D MenuPosition;
		FLinearColor MenuColor;
	} MENU;

	struct {
		float FOV;
		bool GodMode;
		bool SpinBot;
		int NoClip;
	} EXPLOITS;

	struct {
		bool LoadIntoMap;
		bool ShowConsole;
		const char* PlayerName;
		const char* DiscordAppID;
		bool UserScriptsEnabled;
		bool LogProcessEvent;
	} MISC;
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