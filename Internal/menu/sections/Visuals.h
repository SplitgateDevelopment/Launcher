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
			const char* renderers[] = {"UE Canvas", "ImGui (faster)", "None"};
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

			ImGui::SeparatorText("Visibility");
			changed |= ImGui::ToggleButton("Visibility check", &v.EspVisibleCheck);
			ImGui::Tooltip("Recolor visible (recently-rendered) enemies in the Visible color below;\noccluded enemies keep the normal box/bone/snapline colors.");
			changed |= ImGui::ToggleButton("Bot tag", &v.BotTag);
			ImGui::Tooltip("Draw a \"BOT\" label on AI-bot players.");

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

			ImGui::SeparatorText("Crosshair");
			changed |= ImGui::ToggleButton("Crosshair", &v.Crosshair);
			if (v.Crosshair)
			{
				changed |= ImGui::SliderFloat("Size", &v.CrosshairSize, 1.f, 30.f, "%.0f");
				changed |= ImGui::SliderFloat("Gap", &v.CrosshairGap, 0.f, 20.f, "%.0f");
				changed |= ImGui::SliderFloat("Thickness", &v.CrosshairThickness, 1.f, 6.f, "%.0f");
				ImGui::ColorEdit4("Crosshair color", &v.CrosshairColor.R);
				ImGui::Tooltip("Overridden by the RGB rainbow when RGB is on.");
			}

			ImGui::SeparatorText("Bullet traces");
			changed |= ImGui::ToggleButton("Bullet traces", &v.BulletTraces);
			ImGui::Tooltip("Draw a fading trail behind each projectile (PortalWars.Projectile and subclasses).");
			if (v.BulletTraces)
			{
				changed |= ImGui::SliderFloat("Trail duration", &v.BulletTraceDuration, 0.5f, 6.f, "%.1fs");
				ImGui::ColorEdit4("Trail color", &v.BulletTraceColor.R);
				ImGui::Tooltip("Overridden by the RGB rainbow when RGB is on.");
			}

			ImGui::SeparatorText("Glow / chams");
			changed |= ImGui::ToggleButton("Glow enemies", &v.GlowEnemy);
			ImGui::Tooltip("Force a custom-depth outline on enemies, visible through walls.\nRides on the game's team-outline post-process (verify color mapping in-game).");
			if (v.GlowEnemy)
			{
				ImGui::ColorEdit4("Enemy glow", &v.GlowEnemyColor.R);
				ImGui::Tooltip("Overridden by the RGB rainbow when RGB is on.");
			}
			changed |= ImGui::ToggleButton("Glow teammates", &v.GlowFriendly);
			if (v.GlowFriendly)
			{
				ImGui::ColorEdit4("Teammate glow", &v.GlowFriendlyColor.R);
				ImGui::Tooltip("Overridden by the RGB rainbow when RGB is on.");
			}

			ImGui::SeparatorText("Colors");
			ImGui::ColorEdit4("Name", &v.NameColor.R);
			ImGui::ColorEdit4("Box", &v.BoxColor.R);
			ImGui::ColorEdit4("Bones", &v.BonesColor.R);
			ImGui::ColorEdit4("Snaplines", &v.SnaplineColor.R);
			ImGui::ColorEdit4("Friendly", &v.FriendColor.R);
			ImGui::ColorEdit4("Visible", &v.VisibleColor.R);
			ImGui::Tooltip("Color for visible enemies when the visibility check is on.");

			if (changed) Events::Dispatch(Events::Type::SettingsChanged);
		}
	} // namespace Sections
} // namespace Menu
