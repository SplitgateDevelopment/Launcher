#pragma once

#include "../../gui/ZeroGUI.h"
#include "../../settings/Settings.h"
#include "../../scripting/Scripts.h"

namespace Menu {
	namespace Sections {
		void ScriptsTab() {
			ZeroGUI::Checkbox("Enabled", &Settings.MISC.UserScriptsEnabled);
			ZeroGUI::Text("Loaded Scripts:");
			for (const auto& script : Scripts::scriptList) {
				ZeroGUI::Text(std::format(" - {}", script).c_str());
			}
		}
	}
}