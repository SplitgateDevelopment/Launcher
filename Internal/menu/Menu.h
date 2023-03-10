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
				ZeroGUI::Checkbox("Spin Bot", &Settings::SpinBot);

				ZeroGUI::Text("No Clip", false, true);
				ZeroGUI::SameLine();
				ZeroGUI::Hotkey("No Clip", FVector2D{ 110, 25 }, &Settings::NoClip);
				if (ZeroGUI::Button("Load in map", FVector2D{ 110, 25 })) Settings::LoadIntoMap = true;
			}
			else if (tab == 1)
			{
				ZeroGUI::Checkbox("Watermark", &Settings::ShowWatermark);
				//if (ZeroGUI::Button("Destroy Console", FVector2D{ 110, 25 })) Settings::DestroyConsole = true;
				//ZeroGUI::SameLine();
				//if (ZeroGUI::Button("Unload", FVector2D{ 110, 25 })) Settings::Unload = true;
			};
		}
		ZeroGUI::Render();
	};

	void DrawWatermark(Canvas* canvas) {
		if (!Settings::ShowWatermark) return;

		canvas->K2_DrawText(L"github.com/SplitgateDevelopment", { 15.f, 15.f }, { 1.f, 1.f }, ZeroGUI::Colors::Window_Header, 1.f, { 0.f, 0.f, 0.f, 0.f }, { 0.f, 0.f }, false, false, true, ZeroGUI::Colors::Window_Header);
		return;
	};;
};