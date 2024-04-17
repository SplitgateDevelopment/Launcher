#pragma once

#include "../../settings/Settings.h"
#include "../../scripting/Scripts.h"

namespace Menu {
	namespace Sections {
		void ScriptsTab() {
			ImGui::Checkbox("Enabled", &Settings.MISC.UserScriptsEnabled);

            if (ImGui::CollapsingHeader("Loaded Scripts"))
            {
				for (const auto& script : Scripts::scriptList) {
					ImGui::BulletText(script.c_str());
				}
            }
		}
	}
}