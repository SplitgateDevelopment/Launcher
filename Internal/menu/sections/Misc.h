#pragma once

#include "../../gui/ZeroGUI.h"
#include "../../settings/Settings.h"

namespace Menu {
	namespace Sections {
		void MiscTab() {
			ZeroGUI::SliderFloat("Player FOV", &Settings.EXPLOITS.FOV, 80.0f, 160.0f);
			if (ZeroGUI::Button("Load into map", FVector2D{ 110, 25 })) Settings.MISC.LoadIntoMap = true;
		}
	}
}