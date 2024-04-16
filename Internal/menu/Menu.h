#pragma once

#include "../gui/ZeroGUI.h"
#include "../settings/Settings.h"
#include "sections/Misc.h"
#include "sections/Exploits.h"
#include "sections/Scripts.h"
#include "sections/Settings.h"
#include "sections/Watermark.h"

namespace Menu {
	void Tick()
	{
		ZeroGUI::Input::Handle();
		ZeroGUI::Colors::MainColor = Settings.MENU.MenuColor;

		if (GetAsyncKeyState(Settings.MENU.ShowHotkey) & 1) Settings.MENU.ShowMenu = !Settings.MENU.ShowMenu;

		if (ZeroGUI::Window("Splitgate Internal", &Settings.MENU.MenuPosition, FVector2D{ 500.0f, 400.0f }, Settings.MENU.ShowMenu))
		{
			static int tab = 0;
			if (ZeroGUI::ButtonTab("Misc", FVector2D{ 110, 25 }, tab == 0)) tab = 0;
			if (ZeroGUI::ButtonTab("Exploits", FVector2D{ 110, 25 }, tab == 1)) tab = 1;
			if (ZeroGUI::ButtonTab("User Scripts", FVector2D{ 110, 25 }, tab == 2)) tab = 2;
			if (ZeroGUI::ButtonTab("Settings", FVector2D{ 110, 25 }, tab == 2)) tab = 3;
			ZeroGUI::NextColumn(130.0f);

			switch (tab) {
			case 0:
				Sections::MiscTab();
				break;
			case 1:
				Sections::ExploitsTab();
				break;
			case 2:
				Sections::ScriptsTab();
				break;
			case 3:
				Sections::SettingsTab();
				break;
			default:
				break;
			};
		}

		ZeroGUI::Render();
		//ZeroGUI::Draw_Cursor(Settings.MENU.ShowHotkey);
		Sections::Watermark();
	};
};