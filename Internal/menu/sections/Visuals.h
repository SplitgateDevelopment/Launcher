#pragma once

/// @file
/// @brief Visuals tab: player ESP element toggles, radar, and per-element colors.

#include "../../settings/Settings.h"
#include "../../scripting/Events.h"

namespace Menu
{
	namespace Sections
	{
		/// @brief Renders the Visuals tab.
		/// Master ESP toggle plus per-element switches (name, box, 3D box, bones, snaplines, health, distance),
		/// a radar toggle, and color pickers for name/box/bones/snaplines. Dispatches SettingsChanged when any
		/// toggle changes (the color pickers write directly into Settings).
		void VisualsTab()
		{
			bool changed = false;
			auto& v = Settings.VISUALS;

			ImGui::SeparatorText("Player ESP");
			changed |= ImGui::ToggleButton("Enable", &v.Esp);

			ImGui::SeparatorText("Elements");
			changed |= ImGui::ToggleButton("Name", &v.Name);
			changed |= ImGui::ToggleButton("Box", &v.Box);
			changed |= ImGui::ToggleButton("3D Box", &v.Box3D);
			changed |= ImGui::ToggleButton("Bones", &v.Bones);
			changed |= ImGui::ToggleButton("Snaplines", &v.Snaplines);
			changed |= ImGui::ToggleButton("Health", &v.Health);
			changed |= ImGui::ToggleButton("Distance", &v.Distance);

			ImGui::SeparatorText("Radar");
			changed |= ImGui::ToggleButton("Enable Radar", &v.Radar);

			ImGui::SeparatorText("Colors");
			ImGui::ColorEdit4("Name", &v.NameColor.R);
			ImGui::ColorEdit4("Box", &v.BoxColor.R);
			ImGui::ColorEdit4("Bones", &v.BonesColor.R);
			ImGui::ColorEdit4("Snaplines", &v.SnaplineColor.R);

			if (changed) Events::Dispatch(Events::Type::SettingsChanged);
		}
	} // namespace Sections
} // namespace Menu
