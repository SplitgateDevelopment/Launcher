#pragma once

/// @file
/// @brief One-time ImGui IO configuration (nav flags, ini/log file paths).

#include "imgui.h"

namespace GUI
{
	/// @brief ImGui runtime configuration applied once at startup.
	namespace Config
	{
		/// @brief Enables keyboard/gamepad navigation and points ImGui's ini and log files
		/// at app-relative paths (imgui.ini / imgui_log.txt).
		void Init()
		{
			auto iniPath = Shared::AppDataPath(SettingsHelper::AppFolder, "imgui.ini").string();
			auto logsPath = Shared::AppDataPath(SettingsHelper::AppFolder, "imgui_log.txt").string();

			ImGuiIO& io = ImGui::GetIO();
			(void)io;

			io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
			io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

			io.IniFilename = iniPath.c_str();
			io.LogFilename = logsPath.c_str();
		}
	} // namespace Config
} // namespace GUI