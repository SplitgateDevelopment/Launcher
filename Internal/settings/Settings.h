#pragma once

typedef struct {
	struct {
		bool ShowMenu;
		bool ShowWatermark;
		std::string Watermark;
		int ShowHotkey;
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
	} MISC;
} SETTINGS;

extern SETTINGS Settings;

namespace SettingsHelper {
	std::string GetPath();
	bool Init();
	VOID Save();
	VOID Reset();
}