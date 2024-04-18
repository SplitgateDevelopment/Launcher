#pragma once

#include "../../settings/Settings.h"
#include "../../hook/Hook.h"

namespace Menu {
	namespace Sections {
		void DebugTab() {
			ImGui::ToggleButton("Log ProcessEvent", &Settings.DEBUG.LogProcessEvent);
			ImGui::ToggleButton("Show demo window", &Settings.DEBUG.ShowDemoWindow);
			ImGui::ToggleButton("Show style editor", &Settings.DEBUG.ShowStyleEditor);
		}
	}
}