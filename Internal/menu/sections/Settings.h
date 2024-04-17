#pragma once

#include "../../settings/Settings.h"
#include "../../hook/Hook.h"

namespace Menu {
	namespace Sections {
		void SettingsTab() {
			if (ImGui::Button("Save")) {
				SettingsHelper::Save();
			};

			ImGui::SameLine();

			if (ImGui::Button("Reset")) {
				SettingsHelper::Reset();
			};

			ImGui::Text("Menu Hotkey", false, true);
			//ImGui::SameLine();
			//ImGui::Hotkey("Menu Hotkey", &Settings.MENU.ShowHotkey);

			if (ImGui::Button("Toggle Console")) {
				Settings.MISC.ShowConsole = !Settings.MISC.ShowConsole;
				Logger::SetConsoleVisibility(Settings.MISC.ShowConsole);
			};
			ImGui::SameLine();
			if (ImGui::Button("Unload")) {
				//Hook::UnHook();
			}

			//ImGui::ColorEdit3("Menu Color", &Settings.MENU.MenuColor);
			//ImGui::SameLine();
			ImGui::Checkbox("Watermark", &Settings.MENU.ShowWatermark);
		}
	}
}