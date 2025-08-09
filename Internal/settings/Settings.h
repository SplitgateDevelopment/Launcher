#pragma once

#include <filesystem>
#include <ShlObj.h>
#include <fstream>
#include <Windows.h>
#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
namespace fs = std::filesystem;

struct MenuSettings {
    bool ShowMenu = true;
    bool ShowWatermark = true;
    std::string Watermark = "github.com/SplitgateDevelopment/Launcher";
    int ShowHotkey = VK_INSERT;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(MenuSettings, ShowMenu, ShowWatermark, ShowHotkey)

struct ExploitsSettings {
    float FOV = 80.f;
    bool GodMode = false;
    bool SpinBot = false;
    int NoClip = VK_OEM_PLUS;
    bool NoRecoil = false;
    bool GodMelee = false;
    float PlayerSpeed = 1.f;
    bool InfinteJetpack = false;
    bool InfiniteAmmo = false;
    bool NoReload = false;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ExploitsSettings, FOV, GodMode, SpinBot, NoClip, NoRecoil, GodMelee, PlayerSpeed, InfinteJetpack, InfiniteAmmo, NoReload)

struct MiscSettings {
    bool LoadIntoMap = false;
    bool ShowConsole = true;
    std::string PlayerName = "SplitgateDevelopment";
    std::string DiscordAppID = "1078744504066117703";
    bool DiscordRPCEnabled = true;
    std::string SteamAppId = "677620";
    bool UserScriptsEnabled = false;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(MiscSettings, PlayerName, UserScriptsEnabled, DiscordRPCEnabled)

struct DebugSettings {
    bool LogProcessEvent = false;
    bool FeaturesLogging = false;
    bool ShowDemoWindow = false;
    bool ShowStyleEditor = false;
    bool DrawActors = false;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(DebugSettings, LogProcessEvent, FeaturesLogging, ShowDemoWindow, ShowStyleEditor, DrawActors)

struct SETTINGS {
    MenuSettings MENU;
    ExploitsSettings EXPLOITS;
    MiscSettings MISC;
    DebugSettings DEBUG;

    SETTINGS()
        : MENU()
        , EXPLOITS()
        , MISC()
        , DEBUG()
    {
    }
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(SETTINGS, MENU, EXPLOITS, MISC, DEBUG)

extern SETTINGS Settings;

namespace SettingsHelper {
	fs::path GetAppPath(std::string filename = "");
	std::string GetSettingsFilePath();
	bool Load();
	void Save();
	void Reset();
	void Delete();
}