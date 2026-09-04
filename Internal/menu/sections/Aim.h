#pragma once

/// @file
/// @brief Aim tab: aimbot and triggerbot controls.

#include "../../settings/Settings.h"
#include "../../scripting/Events.h"

namespace Menu
{
	namespace Sections
	{
		/// @brief Renders the Aim tab (aimbot + triggerbot). Dispatches SettingsChanged on any change.
		void AimTab()
		{
			bool changed = false;
			auto& a = Settings.AIM;

			ImGui::SeparatorText("Aimbot");
			changed |= ImGui::ToggleButton("Enable##aim", &a.Aimbot);
			ImGui::HotKey("Aim key", &a.AimKey);
			changed |= ImGui::SliderFloat("FOV (px)", &a.AimFov, 10.f, 500.f, "%.0f");
			changed |= ImGui::SliderFloat("Smoothing", &a.AimSmooth, 0.05f, 1.f, "%.2f");
			ImGui::Tooltip("1.0 snaps instantly; lower is smoother.");
			const char* bones[] = {"Head", "Chest", "Pelvis"};
			changed |= ImGui::Combo("Bone", &a.AimBone, bones, IM_ARRAYSIZE(bones));
			changed |= ImGui::ToggleButton("Team check##aim", &a.AimTeamCheck);
			changed |= ImGui::ToggleButton("Silent aim", &a.SilentAim);
			ImGui::Tooltip("Snap to the target only while firing (left click), ignoring the aim key.");
			changed |= ImGui::ToggleButton("Visibility check", &a.AimVisibleCheck);
			ImGui::Tooltip("Only lock onto targets that were recently rendered (visible). Also applies to the triggerbot.");
			changed |= ImGui::ToggleButton("Draw FOV circle", &a.DrawAimFov);
			ImGui::ColorEdit4("FOV circle color", &a.AimFovColor.R);

			ImGui::SeparatorText("Triggerbot");
			changed |= ImGui::ToggleButton("Enable##trig", &a.Triggerbot);
			ImGui::HotKey("Trigger key", &a.TriggerKey);
			changed |= ImGui::SliderFloat("Trigger FOV (px)", &a.TriggerFov, 1.f, 30.f, "%.0f");
			changed |= ImGui::SliderInt("Delay (ms)", &a.TriggerDelay, 0, 500);
			changed |= ImGui::ToggleButton("Team check##trig", &a.TriggerTeamCheck);

			if (changed) Events::Dispatch(Events::Type::SettingsChanged);
		}
	} // namespace Sections
} // namespace Menu
