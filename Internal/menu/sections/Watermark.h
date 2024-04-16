#pragma once

#include "../../gui/ZeroGUI.h"
#include "../../settings/Settings.h"

namespace Menu {
	namespace Sections {
		void Watermark() {
			if (!Settings.MENU.ShowWatermark || !ZeroGUI::canvas) return;

			std::string str = std::string(Settings.MENU.Watermark);
			LPCWSTR watermark = std::wstring(str.begin(), str.end()).c_str();

			ZeroGUI::canvas->K2_DrawText(watermark, { 15.f, 15.f }, { 1.f, 1.f }, ZeroGUI::Colors::MainColor, 1.f, { 0.f, 0.f, 0.f, 0.f }, { 0.f, 0.f }, false, false, true, ZeroGUI::Colors::MainColor);
			return;
		};
	}
}