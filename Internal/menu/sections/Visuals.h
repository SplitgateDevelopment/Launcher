#pragma once

#include "../../settings/Settings.h"
#include "../../scripting/Events.h"

namespace Menu {
	namespace Sections {
		void VisualsTab() {
			bool changed = false;
			auto& v = Settings.VISUALS;

			ImGui::SeparatorText("Player ESP");
			changed |= ImGui::ToggleButton("Enable", &v.Esp);

			ImGui::SeparatorText("Elements");
			changed |= ImGui::ToggleButton("Name", &v.Name);
			changed |= ImGui::ToggleButton("Box", &v.Box);
			changed |= ImGui::ToggleButton("3D Box", &v.Box3D);
			changed |= ImGui::ToggleButton("Bones", &v.Bones);

			ImGui::SeparatorText("Colors");
			ImGui::ColorEdit4("Name", &v.NameColor.R);
			ImGui::ColorEdit4("Box", &v.BoxColor.R);
			ImGui::ColorEdit4("Bones", &v.BonesColor.R);

			if (changed) Events::Dispatch(Events::Type::SettingsChanged);
		}
	}
}
