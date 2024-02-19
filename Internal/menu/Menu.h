#pragma once

#include "../gui/ZeroGUI.h"
#include "../settings/Settings.h"

namespace Menu {
	void DrawWatermark() {
		if (!Settings.MENU.ShowWatermark || !ZeroGUI::canvas) return;

		std::string str = Settings.MENU.Watermark;
		LPCWSTR watermark = std::wstring(str.begin(), str.end()).c_str();

		ZeroGUI::canvas->K2_DrawText(watermark, { 15.f, 15.f }, { 1.f, 1.f }, ZeroGUI::Colors::MainColor, 1.f, { 0.f, 0.f, 0.f, 0.f }, { 0.f, 0.f }, false, false, true, ZeroGUI::Colors::MainColor);
		return;
	};

	void Tick()
	{
		ZeroGUI::Input::Handle();

		if (GetAsyncKeyState(Settings.MENU.ShowHotkey) & 1) Settings.MENU.ShowMenu = !Settings.MENU.ShowMenu;

		if (ZeroGUI::Window("Splitgate Internal", &Settings.MENU.MenuPosition, FVector2D{ 500.0f, 400.0f }, Settings.MENU.ShowMenu))
		{
			static int tab = 0;
			if (ZeroGUI::ButtonTab("Misc", FVector2D{ 110, 25 }, tab == 0)) tab = 0;
			if (ZeroGUI::ButtonTab("Exploits", FVector2D{ 110, 25 }, tab == 1)) tab = 1;
			if (ZeroGUI::ButtonTab("Settings", FVector2D{ 110, 25 }, tab == 2)) tab = 2;
			ZeroGUI::NextColumn(130.0f);

			switch (tab) {
			case 0:
				ZeroGUI::SliderFloat("Player FOV", &Settings.EXPLOITS.FOV, 80.0f, 160.0f);
				if (ZeroGUI::Button("Load into map", FVector2D{ 110, 25 })) Settings.MISC.LoadIntoMap = true;

				break;
			case 1:
				ZeroGUI::Checkbox("God Mode", &Settings.EXPLOITS.GodMode);
				ZeroGUI::Checkbox("Spin Bot", &Settings.EXPLOITS.SpinBot);

				ZeroGUI::Text("No Clip", false, true);
				ZeroGUI::SameLine();
				ZeroGUI::Hotkey("No Clip", FVector2D{ 110, 25 }, &Settings.EXPLOITS.NoClip);

				break;
			case 2:
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
				break;
			default:
				break;
			};
		}

		ZeroGUI::Render();
		DrawWatermark();
	};
};