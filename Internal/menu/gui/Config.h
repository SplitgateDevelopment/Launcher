#pragma once
#include "imgui.h"

namespace GUI {
	namespace Config {
		void Init() {
			auto iniPath = SettingsHelper::GetAppPath("imgui.ini").string();
			auto logsPath = SettingsHelper::GetAppPath("imgui_log.txt").string();

			ImGuiIO& io = ImGui::GetIO(); (void)io;

			io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
			io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

			io.IniFilename = iniPath.c_str();
			io.LogFilename = logsPath.c_str();
		}
	}
}