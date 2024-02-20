#pragma once

#include "../ue/Engine.h"

typedef struct {
	struct {
		bool ShowMenu;
		bool ShowWatermark;
		std::string Watermark;
		int ShowHotkey;
		FVector2D MenuPosition;
	} MENU;

	struct {
		float FOV;
		bool GodMode;
		bool SpinBot;
		int NoClip;
	} EXPLOITS;

	struct {
		bool LoadIntoMap;
		float Unload;
		bool DestroyConsole;
		std::string PlayerName;
		const char* DiscordAppID;
		bool UserScriptsEnabled;
	} MISC;
} SETTINGS;

extern SETTINGS Settings;

namespace SettingsHelper {
	std::string GetPath();
	bool Init();
	VOID Save();
	VOID Reset();
}