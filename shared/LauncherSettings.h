#pragma once

#include <format>
#include <string>

#include <nlohmann/json.hpp>

/**
 * @file
 * @brief Launcher-only settings, deliberately kept out of the DLL's SETTINGS so the internal
 * settings file stays game-focused.
 *
 * The launcher owns `launcher.settings` (Documents\SplitgateInternal\launcher.settings). Today
 * it carries the mitmproxy spawn configuration used when NETWORK.Proxy == Mitmproxy: how the
 * launcher should start `mitmdump`. It is defined here (shared) rather than in the Launcher
 * project so the DLL's Network tab can edit the very struct the launcher reads back at startup.
 */
namespace Shared
{
	/// How the launcher starts `mitmdump` for ProxyMode::Mitmproxy.
	enum class MitmScriptMode
	{
		Default, ///< spawn a generated addon (redirect + TLS passthrough) built from the DLL redirects
		Path,	 ///< run `mitmdump -s <ScriptPath>` with a .py addon already on disk
		Inline,	 ///< write InlineScript (inline python) to a temp file and run it as the addon
	};

	// Persist the enum as a readable string.
	NLOHMANN_JSON_SERIALIZE_ENUM(MitmScriptMode, {
													 {MitmScriptMode::Default, "default"},
													 {MitmScriptMode::Path, "path"},
													 {MitmScriptMode::Inline, "inline"},
												 })

	/// Human-readable name for a script mode (matches the persisted JSON strings above).
	inline const char* ToString(MitmScriptMode mode)
	{
		switch (mode)
		{
		case MitmScriptMode::Default:
			return "default";
		case MitmScriptMode::Path:
			return "path";
		case MitmScriptMode::Inline:
			return "inline";
		}
		return "unknown";
	}

	/// mitmproxy spawn configuration (see MitmScriptMode). ScriptPath and InlineScript are kept
	/// separate so switching modes in the GUI doesn't clobber the other value.
	struct MitmproxySettings
	{
		MitmScriptMode ScriptMode = MitmScriptMode::Default;
		std::string ScriptPath;	  ///< Path mode: filesystem path to a .py addon
		std::string InlineScript; ///< Inline mode: the python source, run as the addon
		bool ShowConsole = false; ///< show mitmdump's console window (default: run it hidden)
	};

	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(MitmproxySettings, ScriptMode, ScriptPath, InlineScript, ShowConsole)

	/// Launcher-owned settings (launcher.settings), separate from the DLL's SETTINGS.
	struct LauncherSettings
	{
		MitmproxySettings MITMPROXY;
	};

	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(LauncherSettings, MITMPROXY)

	/// The launcher's settings file name (lives in the same app folder as the DLL settings).
	inline constexpr const char* LauncherSettingsFileName = "launcher.settings";
} // namespace Shared

/// std::format support so `std::format("{}", mode)` works (scoped enums have no default one).
template <>
struct std::formatter<Shared::MitmScriptMode> : std::formatter<const char*>
{
	auto format(Shared::MitmScriptMode mode, std::format_context& ctx) const
	{
		return std::formatter<const char*>::format(Shared::ToString(mode), ctx);
	}
};
