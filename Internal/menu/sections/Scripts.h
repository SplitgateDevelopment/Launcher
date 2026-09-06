#pragma once

/// @file
/// @brief Scripts tab: enable/disable the user-scripting feature, hot-reload the folder, run a
/// script on demand, and list the loaded scripts. See docs/scripting.md.

#include <string>

#include "../../settings/Settings.h"
#include "../../scripting/Scripts.h"
#include "../../scripting/Events.h"
#include "../ui/UI.h"

namespace Menu
{
	namespace Sections
	{
		/// @brief Renders the Scripts tab.
		void ScriptsTab()
		{
			UI::SeparatorText("User Scripts");
			UI::Tooltip("Python scripts from Documents\\SplitgateInternal\\UserScripts. See docs/scripting.md.");

			if (UI::Toggle("Enable", &Settings.MISC.UserScriptsEnabled))
				Events::Dispatch(Events::Type::SettingsChanged);
			UI::SameLine();
			if (UI::Button("Reload")) Scripts::Reload();
			UI::Tooltip("Re-scan the UserScripts folder and re-import every script (edits take effect\n"
						"without a relaunch). Previously-registered script event handlers are cleared\n"
						"first, so reloading doesn't stack duplicates.");

			UI::SeparatorText("Loaded Scripts");

			// The scrollable list with per-script Run buttons uses ImGui child regions — ImGui-only.
			if (!UI::IsImGui())
			{
				UI::Text("The loaded-script list uses the ImGui menu backend.");
				return;
			}

			if (Scripts::scriptList.empty())
			{
				ImGui::TextDisabled("No scripts found. Drop a .py with a main() into the UserScripts folder, then Reload.");
				return;
			}

			std::string toRun;
			ImGui::BeginChild("ScriptList", ImVec2(0, 200), true);
			for (const auto& script : Scripts::scriptList)
			{
				ImGui::PushID(script.c_str());
				if (ImGui::SmallButton("Run")) toRun = script;
				ImGui::SameLine();
				ImGui::TextUnformatted(script.c_str());
				ImGui::PopID();
			}
			ImGui::EndChild();
			if (!toRun.empty()) Scripts::ExecuteUnloaded(toRun);
		}
	} // namespace Sections
} // namespace Menu
