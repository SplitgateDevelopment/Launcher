#pragma once

#include "../../settings/Settings.h"

namespace Menu {
	namespace Sections {
		void MiscTab() {
			ImGui::SliderFloat("Player FOV", &Settings.EXPLOITS.FOV, 80.0f, 160.0f);
			if (ImGui::Button("Load into map"))
			{
				Settings.MISC.LoadIntoMap = true;
			};
		}
	}
}