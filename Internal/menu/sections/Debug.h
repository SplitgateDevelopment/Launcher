#pragma once

/// @file
/// @brief Debug tab: logging/GUI/crash toggles plus developer actions (summon bot, dump GObjects, list features).

#include "../../settings/Settings.h"
#include "../../scripting/Events.h"
#include "../../hook/Hook.h"

namespace Menu
{
	/// @brief One render function per menu tab; each draws ImGui controls bound to the global Settings
	/// and dispatches Events::Type::SettingsChanged when a control changes.
	namespace Sections
	{
		/// @brief Renders the Debug tab.
		/// Toggles logging (ProcessEvent, features), the ImGui demo/style-editor windows, and delete-settings-on-crash,
		/// dispatching SettingsChanged when any toggle changes. Also provides buttons to summon a bot, dump all
		/// GObjects to Dumps/GObjects.txt, and a tree listing loaded features with their init/enabled state.
		void DebugTab()
		{
			bool changed = false;

			ImGui::SeparatorText("Logging");
			changed |= ImGui::ToggleButton("Log ProcessEvent", &Settings.DEBUG.LogProcessEvent);
			changed |= ImGui::ToggleButton("Features Logging", &Settings.DEBUG.FeaturesLogging);

			ImGui::SeparatorText("GUI");
			changed |= ImGui::ToggleButton("Show demo window", &Settings.DEBUG.ShowDemoWindow);
			changed |= ImGui::ToggleButton("Show style editor", &Settings.DEBUG.ShowStyleEditor);

			ImGui::SeparatorText("Performance");
			changed |= ImGui::ToggleButton("Native WorldToScreen", &Settings.DEBUG.NativeWorldToScreen);
			ImGui::Tooltip("Project overlays with math instead of the game's ProjectWorldLocationToScreen UFunction. Turn off if boxes/names are misplaced.");
			if (!Settings.DEBUG.NativeWorldToScreen)
			{
				changed |= ImGui::ToggleButton("Custom projection", &Settings.DEBUG.CustomProjection);
				ImGui::Tooltip("With native off: use PortalWars' ProjectWorldLocationToScreenCustom instead of the stock UFunction.");
			}
			changed |= ImGui::ToggleButton("Native bones", &Settings.DEBUG.NativeBones);
			ImGui::Tooltip("Project the ESP skeleton via native GetBoneMatrix + WorldToScreen. Off falls back to the game's bone projection.");
			changed |= ImGui::ToggleButton("Native actor location", &Settings.DEBUG.NativeActorLocation);
			ImGui::Tooltip("Read actor location from RootComponent->RelativeLocation (no ProcessEvent). Off uses K2_GetActorLocation.");

			if (changed) Events::Dispatch(Events::Type::SettingsChanged);

			ImGui::SeparatorText("Console command");
			static char consoleBuffer[256] = "";
			ImGui::SetNextItemWidth(260.f);
			ImGui::InputText("##console", consoleBuffer, sizeof(consoleBuffer));
			ImGui::SameLine();
			if (ImGui::Button("Run") && consoleBuffer[0] && Globals::PlayerController)
			{
				Globals::PlayerController->SendToConsole(FString(std::string(consoleBuffer)));
			}

			if (ImGui::TreeNode("Loaded Features"))
			{
				for (const auto& Feature : Features::Features)
				{
					ImGui::BulletText(Feature->Name.c_str());
					ImGui::Tooltip(std::format("Init [{}], Enabled [{}]", Feature->Initialized, Feature->Enabled).c_str());
				}

				ImGui::TreePop();
			}

			if (ImGui::CollapsingHeader("Recent logs"))
			{
				ImGui::BeginChild("Logs", ImVec2(0, 200), true, ImGuiWindowFlags_HorizontalScrollbar);
				for (const auto& line : Logger::Recent())
					ImGui::TextUnformatted(line.c_str());
				if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) ImGui::SetScrollHereY(1.0f); // stick to bottom
				ImGui::EndChild();
			}
		}
	} // namespace Sections
} // namespace Menu