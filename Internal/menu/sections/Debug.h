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

			ImGui::SeparatorText("Crash");
			changed |= ImGui::ToggleButton("Delete settings on crash", &Settings.DEBUG.DeleteSettingsOnCrash);

			if (changed) Events::Dispatch(Events::Type::SettingsChanged);

			if (ImGui::Button("Summon Bot"))
			{
				if (!Globals::PlayerController) return;
				if (!Globals::PlayerController->IsInGame()) return;

				Globals::PlayerController->SendToConsole(FString("summon PortalWarsBot_BP_C"));
				Globals::PlayerController->SendChatMessage(FString("Summoned actor"));
			}

			ImGui::SameLine();

			if (ImGui::Button("Dump GObjects"))
			{
				fs::path dumpsDir = SettingsHelper::GetAppPath() / "Dumps";
				if (!fs::exists(dumpsDir)) fs::create_directories(dumpsDir);

				fs::path filePath = dumpsDir / "GObjects.txt";

				std::ofstream file(filePath, std::ios::out | std::ios::trunc);
				if (!file.is_open())
				{
					char errorMsg[256];
					strerror_s(errorMsg, sizeof(errorMsg), errno);

					Logger::Log("ERROR", std::format("Failed to open {} for writing: {}", filePath.string(), errorMsg));
					return;
				}

				auto objNum = ObjObjects->NumElements;
				for (auto i = 0u; i < objNum; i++)
				{
					auto Object = ObjObjects->GetObjectPtr(i);
					if (!Object) continue;

					file << '[' + std::to_string(Object->InternalIndex) + "] " + Object->GetFullName() << '\n';
				}

				file.close();

				std::string msg = std::format("Dumped {} GObjects to {}", objNum, filePath.string());
				Logger::Log("SUCCESS", msg);
				Globals::PlayerController->SendChatMessage(FString(msg));
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
		}
	} // namespace Sections
} // namespace Menu