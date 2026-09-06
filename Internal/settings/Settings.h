#pragma once

#include <filesystem>
#include <ShlObj.h>
#include <format>
#include <fstream>
#include <map>
#include <Windows.h>
#include <iostream>
#include <nlohmann/json.hpp>

#include "../../shared/Settings.h"

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

/// Plain RGBA color kept out of the UE SDK so settings stay game-independent (and
/// unit-testable). Laid out as four contiguous floats for ImGui::ColorEdit4; the Esp feature
/// converts it to an FLinearColor.
struct Color
{
	float R = 1.f, G = 1.f, B = 1.f, A = 1.f;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(Color, R, G, B, A)

/// How the visual overlays are drawn: through the UE canvas (a ProcessEvent per primitive) or via
/// ImGui/DX11 (near-free), or replayed into a separate capture-excluded window (streamproof).
enum class RendererMode
{
	Canvas,
	ImGui,
	Null,
	External, ///< ImGui recorder, replayed into a separate capture-excluded overlay window (streamproof)
};

NLOHMANN_JSON_SERIALIZE_ENUM(RendererMode, {
											   {RendererMode::Canvas, "canvas"},
											   {RendererMode::ImGui, "imgui"},
											   {RendererMode::Null, "null"},
											   {RendererMode::External, "external"},
										   })

/// Which GUI engine draws the menu itself (independent of RendererMode, which is for the ESP/overlay).
/// ImGui draws in the Present hook; Canvas draws through the UE canvas (UCanvasGUI) in PostRender.
enum class MenuBackend
{
	ImGui,
	Canvas,
};

NLOHMANN_JSON_SERIALIZE_ENUM(MenuBackend, {
											  {MenuBackend::ImGui, "imgui"},
											  {MenuBackend::Canvas, "canvas"},
										  })

/// Menu appearance, the show/hide hotkey, and how the overlays are rendered.
struct MenuSettings
{
	bool ShowMenu = true;
	bool ShowWatermark = true;
	std::string Watermark = "github.com/SplitgateDevelopment/Launcher";
	int ShowHotkey = VK_INSERT;					  ///< virtual-key code toggling the GUI (default Insert)
	bool Rgb = false;							  ///< cycle the watermark, menu accent, and radar self-icon through a rainbow; off = their defaults (red / white)
	RendererMode Renderer = RendererMode::Canvas; ///< how the overlays are drawn (canvas / imgui / null / external)
	MenuBackend Backend = MenuBackend::ImGui;	  ///< which GUI engine draws the menu (imgui / canvas)
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(MenuSettings, ShowMenu, ShowWatermark, ShowHotkey, Rgb, Renderer, Backend)

/// Gameplay feature toggles and tunables (the Exploits tab).
/// Which camera the Camera feature drives. First person is the game default (no override).
enum class CameraMode
{
	FirstPerson, ///< default — no camera override
	ThirdPerson, ///< the game's built-in third-person camera (ClientSetCameraMode)
	FreeCam,	 ///< the game's debug free-fly camera (ToggleDebugCamera)
};

NLOHMANN_JSON_SERIALIZE_ENUM(CameraMode, {
											  {CameraMode::FirstPerson, "first"},
											  {CameraMode::ThirdPerson, "third"},
											  {CameraMode::FreeCam, "free"},
										  })

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
	CameraMode Camera = CameraMode::FirstPerson; ///< first (default) / third / free camera
	float ThirdPersonDistance = 250.f;			 ///< third-person: distance behind the pawn (cm)
	float ThirdPersonHeight = 60.f;				 ///< third-person: camera height above the pawn eye (cm)
	bool EnableAllInput = false;				 ///< force IsInputActionEnabled true (un-greys the Play button, etc.)
	bool PhasingBullets = false; ///< disable collision on cover (CullableActor) so shots pass through it
	bool BulletTp = false;		 ///< teleport your own projectiles onto the target's aim bone (uses the Aim bone/filters)
	bool BulletSpeed = false;	 ///< push your own projectiles along their velocity each frame (faster bullets)
	float BulletSpeedBoost = 500.f; ///< extra cm advanced per frame along the projectile's velocity

	bool SuperJump = false;			 ///< enable the super-jump hotkey
	int SuperJumpKey = 'V';			 ///< pressed to launch upward
	float SuperJumpForce = 1500.f;	 ///< upward launch velocity (cm/s)

	bool Teleport = false;			 ///< enable the teleport-forward hotkey
	int TeleportKey = 'F';			 ///< pressed to teleport toward where you're looking
	float TeleportDistance = 1500.f; ///< how far forward to teleport (cm)
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(ExploitsSettings, FOV, GodMode, SpinBot, NoClip, NoRecoil, GodMelee, PlayerSpeed, InfinteJetpack, InfiniteAmmo, NoReload, Camera, ThirdPersonDistance, ThirdPersonHeight, EnableAllInput, PhasingBullets, BulletTp, BulletSpeed, BulletSpeedBoost, SuperJump, SuperJumpKey, SuperJumpForce, Teleport, TeleportKey, TeleportDistance)

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
	bool AutoSave = false;			///< save settings to disk on every change (SettingsChanged)
	bool AnnounceToggles = false;	///< post a local (client-only) chat line when a feature is toggled
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(MiscSettings, ShowConsole, PlayerName, DiscordRPCEnabled, UserScriptsEnabled, AutoSave, AnnounceToggles)

/// Developer/diagnostic switches (the Debug tab).
struct DebugSettings
{
	bool LogProcessEvent = false;
	bool FeaturesLogging = false;
	bool ShowDemoWindow = false;
	bool ShowStyleEditor = false;
	bool DeleteSettingsOnCrash = true; ///< wipe settings on an unhandled crash so the next launch is clean

	bool NativeWorldToScreen = true; ///< project overlays with math; off falls back to the ProjectWorldLocationToScreen UFunction
	bool NativeBones = true;		 ///< project the ESP skeleton via native GetBoneMatrix + math; off uses the game's bone projection
	bool NativeActorLocation = true; ///< read actor location from RootComponent->RelativeLocation (no ProcessEvent); off uses K2_GetActorLocation
	bool CustomProjection = false;	 ///< when the native math W2S is off, use ProjectWorldLocationToScreenCustom instead of the stock UFunction
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(DebugSettings, LogProcessEvent, FeaturesLogging, ShowDemoWindow, ShowStyleEditor, DeleteSettingsOnCrash, NativeWorldToScreen, NativeBones, NativeActorLocation, CustomProjection)

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
	bool KD = false;		 ///< draw the player's kills/deaths (and killstreak) from their player state
	bool Rank = false;		 ///< draw the player's rank/level (the final ESP text label)
	float MaxDistance = 0.f; ///< only draw enemies within this many metres (0 = unlimited)

	bool EspVisibleCheck = false;	///< recolor enemies that are currently visible (recently rendered) in VisibleColor
	bool BotTag = false;			///< draw a "[BOT]" label before an AI-bot player's name
	bool HideBots = false;			///< don't draw AI bots in the ESP at all

	bool Radar = false;				///< separate 2D radar feature
	bool ShowFriendly = false;		///< ESP: also draw teammates, in FriendColor
	bool RadarShowFriendly = false; ///< radar: also plot teammates, in FriendColor
	bool DrawAllNames = false;		///< debug: draw the object name of every actor (not just players)

	float FontScale = 1.f; ///< text size for ESP names/distance (and the debug names)

	Color NameColor{1.f, 1.f, 1.f, 1.f};
	Color BoxColor{1.f, 0.f, 0.f, 1.f};
	Color BonesColor{0.f, 1.f, 0.f, 1.f};
	Color SnaplineColor{1.f, 1.f, 0.f, 1.f};
	Color FriendColor{0.f, 0.f, 1.f, 1.f}; ///< color for teammates when ShowFriendly is on (default blue)
	Color VisibleColor{0.f, 1.f, 0.f, 1.f}; ///< color for visible enemies when EspVisibleCheck is on (default green)
	Color BotTagColor{0.4f, 0.7f, 1.f, 1.f}; ///< color of the "[BOT]" tag (default light blue)

	bool Crosshair = false;					   ///< draw a custom crosshair at screen centre
	float CrosshairSize = 8.f;				   ///< length of each arm, px
	float CrosshairGap = 4.f;				   ///< gap from centre to each arm, px
	float CrosshairThickness = 1.f;			   ///< line thickness, px
	Color CrosshairColor{0.f, 1.f, 1.f, 1.f};  ///< crosshair color (overridden by RGB when on)

	bool BulletTraces = false;					   ///< draw fading trails behind projectiles
	float BulletTraceDuration = 2.f;			   ///< seconds a trail lingers
	Color BulletTraceColor{1.f, 0.5f, 0.f, 1.f};   ///< trail color (overridden by RGB when on)

	bool GlowEnemy = false;						  ///< force a custom-depth outline (chams) on enemies, visible through walls
	bool GlowFriendly = false;					  ///< same for teammates
	bool GlowSelf = false;						  ///< same for the local player's own pawn (only visible in third person)
	Color GlowEnemyColor{1.f, 0.f, 0.f, 1.f};	  ///< enemy glow color (overridden by RGB when on)
	Color GlowFriendlyColor{0.f, 0.f, 1.f, 1.f};  ///< teammate glow color (overridden by RGB when on)
	Color GlowSelfColor{0.f, 1.f, 0.f, 1.f};	  ///< own-pawn glow color (overridden by RGB when on)
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(VisualsSettings, Esp, Name, Box, Box3D, Bones, Snaplines, Health, Distance, KD, Rank, MaxDistance, EspVisibleCheck, BotTag, HideBots, Radar, ShowFriendly, RadarShowFriendly, DrawAllNames, FontScale, NameColor, BoxColor, BonesColor, SnaplineColor, FriendColor, VisibleColor, BotTagColor, Crosshair, CrosshairSize, CrosshairGap, CrosshairThickness, CrosshairColor, BulletTraces, BulletTraceDuration, BulletTraceColor, GlowEnemy, GlowFriendly, GlowSelf, GlowEnemyColor, GlowFriendlyColor, GlowSelfColor)

/// Aimbot / triggerbot tunables (the Aim tab).
struct AimSettings
{
	bool Aimbot = false;
	int AimKey = VK_RBUTTON;  ///< held down to aim
	float AimFov = 100.f;	  ///< max distance from the crosshair to lock a target, in pixels
	float AimSmooth = 0.5f;	  ///< per-frame aim step: 0.05 (very smooth) .. 1 (instant snap)
	int AimBone = 0;		  ///< target bone: 0 = head, 1 = chest, 2 = pelvis
	bool AimTeamCheck = true; ///< never aim at teammates
	bool SilentAim = false;	  ///< snap to target only while firing (left click), instead of the aim key
	bool AimVisibleCheck = false;			 ///< only lock onto targets that were recently rendered (visible); also used by the triggerbot
	bool AimVisiblePerBone = false;			 ///< strict per-bone line trace: aim at the first bone in line of sight, skip targets with none
	bool IgnoreBots = false;				 ///< aimbot/triggerbot skip AI bots (target only real players)
	bool AimAssist = false;					 ///< amplify the current weapon's built-in aim-assist/magnetism (soft aim)
	float AimAssistStrength = 2.f;			 ///< multiplier applied to the weapon's aim-assist radius/magnetism
	bool DrawAimFov = false;				 ///< draw the aim FOV circle at the crosshair
	Color AimFovColor{1.f, 1.f, 1.f, 1.f};	 ///< aim FOV circle color

	bool Triggerbot = false;
	int TriggerKey = VK_XBUTTON2; ///< held down to enable the triggerbot
	float TriggerFov = 6.f;		  ///< fire when a target is within this many pixels of the crosshair
	int TriggerDelay = 40;		  ///< ms to wait after acquiring a target before firing
	bool TriggerTeamCheck = true;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(AimSettings, Aimbot, AimKey, AimFov, AimSmooth, AimBone, AimTeamCheck, SilentAim, AimVisibleCheck, AimVisiblePerBone, IgnoreBots, AimAssist, AimAssistStrength, DrawAimFov, AimFovColor, Triggerbot, TriggerKey, TriggerFov, TriggerDelay, TriggerTeamCheck)

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

/// Human-readable name for a proxy mode (matches the persisted JSON strings above).
inline const char* ToString(ProxyMode mode)
{
	switch (mode)
	{
	case ProxyMode::Manual:
		return "manual";
	case ProxyMode::Internal:
		return "internal";
	case ProxyMode::Mitmproxy:
		return "mitmproxy";
	}
	return "unknown";
}

/// std::format support so `std::format("{}", proxyMode)` works (there's no default formatter
/// for a scoped enum).
template <>
struct std::formatter<ProxyMode> : std::formatter<const char*>
{
	auto format(ProxyMode mode, std::format_context& ctx) const
	{
		return std::formatter<const char*>::format(ToString(mode), ctx);
	}
};

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
	bool BypassSslVerify = false;		///< force curl's CURLOPT_SSL_VERIFYPEER/VERIFYHOST off so a redirected host can use a self-signed cert (disables TLS verification for ALL curl traffic)
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(NetworkSettings, Proxy, Redirects, HttpLogging, HttpLogToFile, HttpLogRedirectedOnly, BypassSslVerify)

/// Root settings object — the sections that persist together as one JSON document.
struct SETTINGS
{
	MenuSettings MENU;
	ExploitsSettings EXPLOITS;
	MiscSettings MISC;
	DebugSettings DEBUG;
	VisualsSettings VISUALS;
	AimSettings AIM;
	NetworkSettings NETWORK;

	SETTINGS()
		: MENU(), EXPLOITS(), MISC(), DEBUG(), VISUALS(), AIM(), NETWORK()
	{
	}
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(SETTINGS, MENU, EXPLOITS, MISC, DEBUG, VISUALS, AIM, NETWORK)

/// The one global settings instance (defined in Settings.cpp).
extern SETTINGS Settings;

/// The app-data location and the settings file for the global @ref Settings. Persistence is
/// done through the returned @ref Shared::SettingsFile (Load/Save/Reset/Remove); general
/// app-data paths come from `Shared::AppDataPath(AppFolder, ...)`.
namespace SettingsHelper
{
	/// App-data folder and settings file name. Single source of truth — the DLL owns the file,
	/// the launcher reads it (Launcher/utils/ProxyConfig.h), so both reference these.
	inline constexpr const char* AppFolder = "SplitgateInternal";
	inline constexpr const char* SettingsFileName = "settings/internal.settings";

	/// The settings file bound to the global @ref Settings (lazily created on first use). Use
	/// its `Load()` / `Save()` / `Reset()` / `Remove()` for persistence and `Path()` for its path.
	Shared::SettingsFile<SETTINGS>& File();
} // namespace SettingsHelper