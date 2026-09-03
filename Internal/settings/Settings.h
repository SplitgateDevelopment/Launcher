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

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(MenuSettings, ShowMenu, ShowWatermark, ShowHotkey)

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
    bool ThirdPerson = false;
    bool FreeCam = false;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(ExploitsSettings, FOV, GodMode, SpinBot, NoClip, NoRecoil, GodMelee, PlayerSpeed, InfinteJetpack, InfiniteAmmo, NoReload, ThirdPerson, FreeCam)

struct MiscSettings {
    bool ShowConsole = true;
    std::string PlayerName = "SplitgateDevelopment";
    std::string DiscordAppID = "1078744504066117703";
    bool DiscordRPCEnabled = true;
    std::string SteamAppId = "677620";
    bool UserScriptsEnabled = false;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(MiscSettings, ShowConsole, PlayerName, DiscordRPCEnabled, UserScriptsEnabled)

struct DebugSettings {
    bool LogProcessEvent = false;
    bool FeaturesLogging = false;
    bool ShowDemoWindow = false;
    bool ShowStyleEditor = false;
    bool DeleteSettingsOnCrash = true;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(DebugSettings, LogProcessEvent, FeaturesLogging, ShowDemoWindow, ShowStyleEditor, DeleteSettingsOnCrash)

// Plain RGBA color kept out of the UE SDK so settings stay game-independent
// (and unit-testable). Laid out as four contiguous floats for ImGui::ColorEdit4;
// the Esp feature converts it to an FLinearColor.
struct Color {
    float R = 1.f, G = 1.f, B = 1.f, A = 1.f;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(Color, R, G, B, A)

struct VisualsSettings {
    bool Esp = false;    // master toggle for the ESP feature
    bool Name = true;
    bool Box = true;
    bool Box3D = false;  // false = 2D box, true = 3D box
    bool Bones = false;
    bool Snaplines = false;
    bool Health = false;
    bool Distance = false;

    bool Radar = false;  // separate 2D radar feature

    Color NameColor{ 1.f, 1.f, 1.f, 1.f };
    Color BoxColor{ 1.f, 0.f, 0.f, 1.f };
    Color BonesColor{ 0.f, 1.f, 0.f, 1.f };
    Color SnaplineColor{ 1.f, 1.f, 0.f, 1.f };
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(VisualsSettings, Esp, Name, Box, Box3D, Bones, Snaplines, Health, Distance, Radar, NameColor, BoxColor, BonesColor, SnaplineColor)

struct SETTINGS {
    MenuSettings MENU;
    ExploitsSettings EXPLOITS;
    MiscSettings MISC;
    DebugSettings DEBUG;
    VisualsSettings VISUALS;

    SETTINGS()
        : MENU()
        , EXPLOITS()
        , MISC()
        , DEBUG()
        , VISUALS()
    {
    }
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(SETTINGS, MENU, EXPLOITS, MISC, DEBUG, VISUALS)

extern SETTINGS Settings;

namespace SettingsHelper {
	fs::path GetAppPath(std::string filename = "");
	std::string GetSettingsFilePath();
	bool Load();
	void Save();
	void Reset();
	void Delete();
}