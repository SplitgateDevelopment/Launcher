#pragma once

#include "../gui/ZeroGUI.h"
#include "Settings.h"

class Menu {
private:
	FVector2D pos;
public:
	void Tick()
	{
		ZeroGUI::Input::Handle();

		if (GetAsyncKeyState(VK_INSERT) & 1) Settings::ShowMenu = !Settings::ShowMenu;

		if (ZeroGUI::Window("Splitgate Internal", &pos, FVector2D{ 500.0f, 400.0f }, Settings::ShowMenu))
		{
			static int tab = 0;
			if (ZeroGUI::ButtonTab("Misc", FVector2D{ 110, 25 }, tab == 0)) tab = 0;
			if (ZeroGUI::ButtonTab("Settings", FVector2D{ 110, 25 }, tab == 1)) tab = 1;
			ZeroGUI::NextColumn(130.0f);

			if (tab == 0)
			{
				ZeroGUI::SliderFloat("Player FOV", &Settings::FOV, 80.0f, 160.0f);
				ZeroGUI::Checkbox("God Mode", &Settings::GodMode);
				if (ZeroGUI::Button("Load in map", FVector2D{ 110, 25 })) Settings::LoadIntoMap = true;
			}
			else if (tab == 1)
			{
				ZeroGUI::Button("Unload", FVector2D{ 110, 25 });
			};
		}
		ZeroGUI::Render();
	}
};