#pragma once

/// @file
/// @brief One-time ImGui IO configuration (nav flags, ini/log file paths).

#include "imgui.h"

namespace GUI
{
	/// @brief ImGui runtime configuration applied once at startup.
	namespace Config
	{
		/// @brief Enables keyboard/gamepad navigation and points ImGui's ini and log files at
		/// app-relative paths (imgui.ini, and the log alongside internal.log in logs/imgui.log).
		void Init()
		{
			// ImGui keeps the raw const char* we hand it (it reads IniFilename/LogFilename later, on
			// save and when logging), so the backing strings must outlive this call — hence static.
			// AppDataPath also creates the parent dir, so logs/ exists for imgui.log.
			static const std::string iniPath = Shared::AppDataPath(SettingsHelper::AppFolder, "settings/imgui.ini").string();
			static const std::string logPath = Shared::AppDataPath(SettingsHelper::AppFolder, "logs/imgui.log").string();

			ImGuiIO& io = ImGui::GetIO();
			(void)io;

			io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
			io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

			io.IniFilename = iniPath.c_str();
			io.LogFilename = logPath.c_str();
		}
	} // namespace Config
} // namespace GUI