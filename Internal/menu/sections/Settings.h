#pragma once

#include "../../gui/ZeroGUI.h"
#include "../../settings/Settings.h"
#include "../../hook/Hook.h"

namespace Menu {
	namespace Sections {
		void SettingsTab() {
			if (ZeroGUI::Button("Save", FVector2D{ 110, 25 })) {
				SettingsHelper::Save();
			};

			ZeroGUI::SameLine();

			if (ZeroGUI::Button("Reset", FVector2D{ 110, 25 })) {
				SettingsHelper::Reset();
			};

			ZeroGUI::Text("Menu Hotkey", false, true);
			ZeroGUI::SameLine();
			ZeroGUI::Hotkey("Menu Hotkey", FVector2D{ 110, 25 }, &Settings.MENU.ShowHotkey);

			if (ZeroGUI::Button("Toggle Console", FVector2D{ 110, 25 })) {
				Settings.MISC.ShowConsole = !Settings.MISC.ShowConsole;
				Logger::SetConsoleVisibility(Settings.MISC.ShowConsole);
			};
			ZeroGUI::SameLine();
			if (ZeroGUI::Button("Unload", FVector2D{ 110, 25 })) {
				Hook::UnHook();
			}

			ZeroGUI::ColorPicker("Menu Color", &Settings.MENU.MenuColor);
			ZeroGUI::SameLine();
			ZeroGUI::Checkbox("Watermark", &Settings.MENU.ShowWatermark);

			ZeroGUI::Checkbox("Log ProcessEvent", &Settings.MISC.LogProcessEvent);
		}
	}
}