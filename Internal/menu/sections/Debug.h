#pragma once

/// @file
/// @brief Debug tab: logging/GUI/crash toggles plus developer actions (summon bot, dump GObjects, list features).

#include "../../settings/Settings.h"
#include "../../scripting/Events.h"
#include "../../hook/Hook.h"
#include "../../hook/GuardHook.h"
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
			UI::SeparatorText("Logging");
			UI::ToggleSetting("Log ProcessEvent", &Settings.DEBUG.LogProcessEvent);
			UI::ToggleSetting("Features Logging", &Settings.DEBUG.FeaturesLogging);

			UI::SeparatorText("GUI");
			UI::ToggleSetting("Show demo window", &Settings.DEBUG.ShowDemoWindow);
			UI::ToggleSetting("Show style editor", &Settings.DEBUG.ShowStyleEditor);

			UI::SeparatorText("Performance");
			UI::ToggleSetting("Native WorldToScreen", &Settings.DEBUG.NativeWorldToScreen);
			UI::Tooltip("Project overlays with math instead of the game's ProjectWorldLocationToScreen UFunction. Turn off if boxes/names are misplaced.");
			if (!Settings.DEBUG.NativeWorldToScreen)
			{
				UI::ToggleSetting("Custom projection", &Settings.DEBUG.CustomProjection);
				UI::Tooltip("With native off: use PortalWars' ProjectWorldLocationToScreenCustom instead of the stock UFunction.");
			}
			UI::ToggleSetting("Native bones", &Settings.DEBUG.NativeBones);
			UI::Tooltip("Project the ESP skeleton via native GetBoneMatrix + WorldToScreen. Off falls back to the game's bone projection.");
			UI::ToggleSetting("Native actor location", &Settings.DEBUG.NativeActorLocation);
			UI::Tooltip("Read actor location from RootComponent->RelativeLocation (no ProcessEvent). Off uses K2_GetActorLocation.");

			UI::SeparatorText("Files");
			if (UI::Button("Open app folder"))
				Shared::Utilities::OpenFolder(Shared::AppDataPath(SettingsHelper::AppFolder));
			UI::Tooltip("Open the SplitgateInternal data folder (settings, logs, dumps).");

			UI::SeparatorText("Hooking");
			if (UI::Button("Test GuardHook"))
			{
				const bool ok = Hook::GuardHook::SelfTest();
				Logger::Log(ok ? "SUCCESS" : "ERROR", ok ? "[GuardHook] self-test passed (detour ran)" : "[GuardHook] self-test failed");
			}
			UI::Tooltip("Run an in-process self-test of the guard-page hook primitive: it hooks a scratch\nfunction and confirms the detour ran instead. Touches no game code; result goes to the log.");

			UI::SeparatorText("Console command");
			static char consoleBuffer[256] = "";
			UI::SetNextItemWidth(260.f);
			UI::InputText("##console", consoleBuffer, sizeof(consoleBuffer));
			UI::SameLine();
			if (UI::Button("Run") && consoleBuffer[0] && Engine::PlayerController)
			{
				Engine::PlayerController->SendToConsole(FString(std::string(consoleBuffer)));
			}

			if (UI::TreeNode("Loaded Features"))
			{
				for (const auto& Feature : Features::Features)
				{
					UI::BulletText("%s", Feature->Name.c_str());
					UI::Tooltip(std::format("Init [{}], Enabled [{}]", Feature->Initialized, Feature->Enabled).c_str());
				}

				UI::TreePop();
			}

			if (UI::CollapsingHeader("Recent logs"))
			{
				if (UI::Button("Copy##logs"))
				{
					std::string out;
					for (const auto& line : Logger::Recent())
						out += line + "\n";
					UI::SetClipboardText(out.c_str());
				}
				UI::Tooltip("Copy the recent log lines to the clipboard.");

				UI::BeginChild("Logs", 0, 200);
				for (const auto& line : Logger::Recent())
					UI::Text("%s", line.c_str());
				UI::EndChild();
			}
		}
	} // namespace Sections
} // namespace Menu