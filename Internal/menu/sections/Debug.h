#pragma once

#include "../../settings/Settings.h"
#include "../../hook/Hook.h"

namespace Menu {
	namespace Sections {
		void DebugTab() {
			ImGui::Checkbox("Log ProcessEvent", &Settings.DEBUG.LogProcessEvent);
			ImGui::Checkbox("Show demo window", &Settings.DEBUG.ShowDemoWindow);
		}
	}
}