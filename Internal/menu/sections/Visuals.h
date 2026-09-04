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

			ImGui::SeparatorText("Renderer");
			const char* renderers[] = {"UE Canvas", "ImGui (faster)"};
			int renderer = static_cast<int>(v.Renderer);
			if (ImGui::Combo("Draw with", &renderer, renderers, IM_ARRAYSIZE(renderers)))
			{
				v.Renderer = static_cast<RendererMode>(renderer);
				changed = true;
			}
			ImGui::Tooltip("ImGui draws the overlay without a ProcessEvent per line/text - much faster for a busy ESP.");

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

			ImGui::SeparatorText("Teams");
			changed |= ImGui::ToggleButton("Show teammates", &v.ShowFriendly);
			ImGui::Tooltip("Also draw teammates (ESP + radar), in the friendly color below.");

			ImGui::SeparatorText("Radar");
			changed |= ImGui::ToggleButton("Enable Radar", &v.Radar);
			changed |= ImGui::ToggleButton("Radar teammates", &v.RadarShowFriendly);

			ImGui::SeparatorText("Debug");
			changed |= ImGui::ToggleButton("Draw all object names", &v.DrawAllNames);
			ImGui::Tooltip("Draws the UObject name of every actor in the world (not just players).");

			ImGui::SeparatorText("Text");
			changed |= ImGui::SliderFloat("Font size", &v.FontScale, 0.5f, 3.f, "%.2f");

			ImGui::SeparatorText("Colors");
			ImGui::ColorEdit4("Name", &v.NameColor.R);
			ImGui::ColorEdit4("Box", &v.BoxColor.R);
			ImGui::ColorEdit4("Bones", &v.BonesColor.R);
			ImGui::ColorEdit4("Snaplines", &v.SnaplineColor.R);
			ImGui::ColorEdit4("Friendly", &v.FriendColor.R);

			if (changed) Events::Dispatch(Events::Type::SettingsChanged);
		}
	} // namespace Sections
} // namespace Menu
