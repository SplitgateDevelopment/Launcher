#pragma once

#include <filesystem>
#include <ShlObj.h>
#include <fstream>
#include <map>
#include <Windows.h>
#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
namespace fs = std::filesystem;

/**
 * @file
 * @brief Persisted configuration.
 *
 * A tree of plain settings structs (MENU/EXPLOITS/MISC/DEBUG/VISUALS) serialized to JSON via
 * nlohmann. Each struct's NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT lists only the
 * fields that persist — fields left out of the macro are runtime-only (e.g. DiscordAppID,
 * SteamAppId), and missing keys fall back to the defaults here. `SettingsHelper` resolves the
 * on-disk path and loads/saves/resets the global @ref Settings instance.
 */

/// Menu appearance and the show/hide hotkey.
struct MenuSettings
{
	bool ShowMenu = true;
	bool ShowWatermark = true;
	std::string Watermark = "github.com/SplitgateDevelopment/Launcher";
	int ShowHotkey = VK_INSERT; ///< virtual-key code toggling the GUI (default Insert)
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(MenuSettings, ShowMenu, ShowWatermark, ShowHotkey)

/// Gameplay feature toggles and tunables (the Exploits tab).
struct ExploitsSettings
{
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
	bool EnableAllInput = false; ///< force IsInputActionEnabled true (un-greys the Play button, etc.)
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(ExploitsSettings, FOV, GodMode, SpinBot, NoClip, NoRecoil, GodMelee, PlayerSpeed, InfinteJetpack, InfiniteAmmo, NoReload, ThirdPerson, FreeCam, EnableAllInput)

/// Miscellaneous options. Note DiscordAppID and SteamAppId are runtime-only (absent from the
/// persistence macro below), so they always reset to these defaults.
struct MiscSettings
{
	bool ShowConsole = true;
	std::string PlayerName = "SplitgateDevelopment";
	std::string DiscordAppID = "1078744504066117703"; ///< runtime-only (not persisted)
	bool DiscordRPCEnabled = true;
	std::string SteamAppId = "677620"; ///< runtime-only (not persisted)
	bool UserScriptsEnabled = false;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(MiscSettings, ShowConsole, PlayerName, DiscordRPCEnabled, UserScriptsEnabled)

/// Developer/diagnostic switches (the Debug tab).
struct DebugSettings
{
	bool LogProcessEvent = false;
	bool FeaturesLogging = false;
	bool ShowDemoWindow = false;
	bool ShowStyleEditor = false;
	bool DeleteSettingsOnCrash = true; ///< wipe settings on an unhandled crash so the next launch is clean
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(DebugSettings, LogProcessEvent, FeaturesLogging, ShowDemoWindow, ShowStyleEditor, DeleteSettingsOnCrash)

/// Plain RGBA color kept out of the UE SDK so settings stay game-independent (and
/// unit-testable). Laid out as four contiguous floats for ImGui::ColorEdit4; the Esp feature
/// converts it to an FLinearColor.
struct Color
{
	float R = 1.f, G = 1.f, B = 1.f, A = 1.f;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(Color, R, G, B, A)

/// ESP element toggles and colors (the Visuals tab), plus the separate radar toggle.
struct VisualsSettings
{
	bool Esp = false; ///< master toggle for the ESP feature
	bool Name = true;
	bool Box = true;
	bool Box3D = false; ///< false = 2D box, true = 3D box
	bool Bones = false;
	bool Snaplines = false;
	bool Health = false;
	bool Distance = false;

	bool Radar = false; ///< separate 2D radar feature

	Color NameColor{1.f, 1.f, 1.f, 1.f};
	Color BoxColor{1.f, 0.f, 0.f, 1.f};
	Color BonesColor{0.f, 1.f, 0.f, 1.f};
	Color SnaplineColor{1.f, 1.f, 0.f, 1.f};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(VisualsSettings, Esp, Name, Box, Box3D, Bones, Snaplines, Health, Distance, Radar, NameColor, BoxColor, BonesColor, SnaplineColor)

/// How the game's backend traffic is redirected to the private server.
enum class ProxyMode
{
	Manual,	   ///< do nothing (use an external proxy yourself, or none)
	Internal,  ///< the injected DLL redirects in-process (network/ hooks)
	Mitmproxy, ///< the launcher spawns mitmproxy to the target before the game starts
};

// Persist the enum as a readable string.
NLOHMANN_JSON_SERIALIZE_ENUM(ProxyMode, {
											{ProxyMode::Manual, "manual"},
											{ProxyMode::Internal, "internal"},
											{ProxyMode::Mitmproxy, "mitmproxy"},
										})

/// Networking — backend redirection and HTTP logging (see the `network/` module and
/// docs/backend-redirect.md). Configurable from the Network tab; read by the launcher too.
struct NetworkSettings
{
	ProxyMode Proxy = ProxyMode::Manual; ///< which redirection mechanism to use
	/// original host -> "host[:port]" to route it to. https is downgraded to http on redirect.
	std::map<std::string, std::string> Redirects = {
		{"splitgate.accelbyte.io", "127.0.0.1:5005"},
	};

	bool HttpLogging = false;			///< log outgoing HTTP calls to the console
	bool HttpLogToFile = false;			///< also mirror the HTTP log to http.log
	bool HttpLogRedirectedOnly = false; ///< only log calls whose host is a redirect key
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(NetworkSettings, Proxy, Redirects, HttpLogging, HttpLogToFile, HttpLogRedirectedOnly)

/// Root settings object — the sections that persist together as one JSON document.
struct SETTINGS
{
	MenuSettings MENU;
	ExploitsSettings EXPLOITS;
	MiscSettings MISC;
	DebugSettings DEBUG;
	VisualsSettings VISUALS;
	NetworkSettings NETWORK;

	SETTINGS()
		: MENU(), EXPLOITS(), MISC(), DEBUG(), VISUALS(), NETWORK()
	{
	}
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(SETTINGS, MENU, EXPLOITS, MISC, DEBUG, VISUALS, NETWORK)

/// The one global settings instance (defined in Settings.cpp).
extern SETTINGS Settings;

/// On-disk persistence for the global @ref Settings: path resolution plus load/save/reset/delete.
namespace SettingsHelper
{
	/// @param filename optional leaf to append.
	/// @return the app data folder (Documents\SplitgateInternal), with @p filename appended if given.
	fs::path GetAppPath(std::string filename = "");
	/// @return the full path to the settings JSON file.
	std::string GetSettingsFilePath();
	/// Loads the settings file into @ref Settings, keeping defaults for missing/invalid keys.
	/// @return true if a file was read, false if none existed (or it couldn't be opened).
	bool Load();
	/// Serializes @ref Settings to disk.
	void Save();
	/// Resets @ref Settings to defaults in memory and saves.
	void Reset();
	/// Deletes the settings file from disk (used on crash recovery).
	void Delete();
} // namespace SettingsHelper