#pragma once

#include "../../gui/ZeroGUI.h"
#include "../../settings/Settings.h"

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

			if (ZeroGUI::Button("Detach Console", FVector2D{ 110, 25 })) Settings.MISC.DestroyConsole = true;
			ZeroGUI::SameLine();
			if (ZeroGUI::Button("Unload", FVector2D{ 110, 25 })) Settings.MISC.Unload = true;

			ZeroGUI::ColorPicker("Menu Color", &ZeroGUI::Colors::MainColor);
			ZeroGUI::SameLine();
			ZeroGUI::Checkbox("Watermark", &Settings.MENU.ShowWatermark);
		}
	}
}