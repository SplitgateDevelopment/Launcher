#pragma once

/// @file
/// @brief Aim tab: aimbot and triggerbot controls.

#include "../../settings/Settings.h"
#include "../../scripting/Events.h"
#include "../ui/UI.h"

namespace Menu
{
	namespace Sections
	{
		/// @brief Renders the Aim tab (aimbot + triggerbot). Dispatches SettingsChanged on any change.
		void AimTab()
		{
			bool changed = false;
			auto& a = Settings.AIM;

			UI::SeparatorText("Aimbot");
			changed |= UI::Toggle("Enable##aim", &a.Aimbot);
			UI::HotKey("Aim key", &a.AimKey);
			changed |= UI::SliderFloat("FOV (px)", &a.AimFov, 10.f, 500.f, "%.0f");
			changed |= UI::SliderFloat("Smoothing", &a.AimSmooth, 0.05f, 1.f, "%.2f");
			UI::Tooltip("1.0 snaps instantly; lower is smoother.");
			const char* bones[] = {"Head", "Chest", "Pelvis"};
			changed |= UI::Combo("Bone", &a.AimBone, bones, IM_ARRAYSIZE(bones));
			changed |= UI::Toggle("Team check##aim", &a.AimTeamCheck);
			changed |= UI::Toggle("Ignore bots", &a.IgnoreBots);
			UI::Tooltip("Aimbot and triggerbot target only real players, never AI bots.");
			changed |= UI::Toggle("Silent aim", &a.SilentAim);
			UI::Tooltip("Snap to the target only while firing (left click), ignoring the aim key.");
			changed |= UI::Toggle("Visibility check", &a.AimVisibleCheck);
			UI::Tooltip("Only lock onto targets that were recently rendered (visible). Also applies to the triggerbot.");
			if (a.AimVisibleCheck)
			{
				changed |= UI::Toggle("Per-bone visibility", &a.AimVisiblePerBone);
				UI::Tooltip("Stricter: line-trace each bone and aim at the first one in line of sight\n(e.g. skip the head when only the legs are exposed). Skips targets with no visible bone.");
			}
			changed |= UI::Toggle("Aim assist", &a.AimAssist);
			UI::Tooltip("Amplify the weapon's built-in aim-assist/magnetism (soft aim, view isn't moved).\nMay only take effect on controller input - verify in-game.");
			if (a.AimAssist)
				changed |= UI::SliderFloat("Aim assist strength", &a.AimAssistStrength, 1.f, 8.f, "%.1fx");

			changed |= UI::Toggle("Draw FOV circle", &a.DrawAimFov);
			UI::ColorEdit("FOV circle color", &a.AimFovColor);

			UI::SeparatorText("Triggerbot");
			changed |= UI::Toggle("Enable##trig", &a.Triggerbot);
			UI::HotKey("Trigger key", &a.TriggerKey);
			changed |= UI::SliderFloat("Trigger FOV (px)", &a.TriggerFov, 1.f, 30.f, "%.0f");
			changed |= UI::SliderInt("Delay (ms)", &a.TriggerDelay, 0, 500);
			changed |= UI::Toggle("Team check##trig", &a.TriggerTeamCheck);

			if (changed) Events::Dispatch(Events::Type::SettingsChanged);
		}
	} // namespace Sections
} // namespace Menu
