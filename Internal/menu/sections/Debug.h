#pragma once

/// @file
/// @brief Debug tab: logging/GUI/crash toggles plus developer actions (summon bot, dump GObjects, list features).

#include "../../settings/Settings.h"
#include "../../scripting/Events.h"
#include "../../hook/Hook.h"
#include "../../../shared/Utilities.h"
#include "../ui/UI.h"

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

			UI::SeparatorText("Logging");
			changed |= UI::Toggle("Log ProcessEvent", &Settings.DEBUG.LogProcessEvent);
			changed |= UI::Toggle("Features Logging", &Settings.DEBUG.FeaturesLogging);

			UI::SeparatorText("GUI");
			changed |= UI::Toggle("Show demo window", &Settings.DEBUG.ShowDemoWindow);
			changed |= UI::Toggle("Show style editor", &Settings.DEBUG.ShowStyleEditor);

			UI::SeparatorText("Performance");
			changed |= UI::Toggle("Native WorldToScreen", &Settings.DEBUG.NativeWorldToScreen);
			UI::Tooltip("Project overlays with math instead of the game's ProjectWorldLocationToScreen UFunction. Turn off if boxes/names are misplaced.");
			if (!Settings.DEBUG.NativeWorldToScreen)
			{
				changed |= UI::Toggle("Custom projection", &Settings.DEBUG.CustomProjection);
				UI::Tooltip("With native off: use PortalWars' ProjectWorldLocationToScreenCustom instead of the stock UFunction.");
			}
			changed |= UI::Toggle("Native bones", &Settings.DEBUG.NativeBones);
			UI::Tooltip("Project the ESP skeleton via native GetBoneMatrix + WorldToScreen. Off falls back to the game's bone projection.");
			changed |= UI::Toggle("Native actor location", &Settings.DEBUG.NativeActorLocation);
			UI::Tooltip("Read actor location from RootComponent->RelativeLocation (no ProcessEvent). Off uses K2_GetActorLocation.");

			if (changed) Events::Dispatch(Events::Type::SettingsChanged);

			UI::SeparatorText("Files");
			if (UI::Button("Open app folder"))
				Shared::Utilities::OpenFolder(Shared::AppDataPath(SettingsHelper::AppFolder));
			UI::Tooltip("Open the SplitgateInternal data folder (settings, logs, dumps).");

			// The console-command input, feature tree and log child region use ImGui InputText /
			// TreeNode / child regions — ImGui-only. The Canvas backend shows a note.
			if (!UI::IsImGui())
			{
				UI::Text("Console command, feature tree and logs use the ImGui menu backend.");
				return;
			}

			ImGui::SeparatorText("Console command");
			static char consoleBuffer[256] = "";
			ImGui::SetNextItemWidth(260.f);
			ImGui::InputText("##console", consoleBuffer, sizeof(consoleBuffer));
			ImGui::SameLine();
			if (ImGui::Button("Run") && consoleBuffer[0] && Engine::PlayerController)
			{
				Engine::PlayerController->SendToConsole(FString(std::string(consoleBuffer)));
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
				if (ImGui::Button("Copy##logs"))
				{
					std::string out;
					for (const auto& line : Logger::Recent())
						out += line + "\n";
					ImGui::SetClipboardText(out.c_str());
				}
				ImGui::Tooltip("Copy the recent log lines to the clipboard.");

				ImGui::BeginChild("Logs", ImVec2(0, 200), true, ImGuiWindowFlags_HorizontalScrollbar);
				for (const auto& line : Logger::Recent())
					ImGui::TextUnformatted(line.c_str());
				if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) ImGui::SetScrollHereY(1.0f); // stick to bottom
				ImGui::EndChild();
			}
		}
	} // namespace Sections
} // namespace Menu