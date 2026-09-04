#pragma once

/// @file
/// @brief Scripts tab: enable/disable the user-scripting feature, hot-reload the folder, run a
/// script on demand, and list the loaded scripts. See docs/scripting.md.

#include <string>

#include "../../settings/Settings.h"
#include "../../scripting/Scripts.h"
#include "../../scripting/Events.h"

namespace Menu
{
	namespace Sections
	{
		/// @brief Renders the Scripts tab.
		void ScriptsTab()
		{
			ImGui::SeparatorText("User Scripts");
			ImGui::Tooltip("Python scripts from Documents\\SplitgateInternal\\UserScripts. See docs/scripting.md.");

			if (ImGui::ToggleButton("Enable", &Settings.MISC.UserScriptsEnabled))
				Events::Dispatch(Events::Type::SettingsChanged);
			ImGui::SameLine();
			if (ImGui::Button("Reload")) Scripts::Reload();
			ImGui::Tooltip("Re-scan the UserScripts folder and re-import every script (edits take effect\n"
						   "without a relaunch). Scripts that subscribe to bus events at import time will\n"
						   "stack duplicate handlers — prefer the per-frame main() model for those.");

			ImGui::SeparatorText("Loaded Scripts");
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
