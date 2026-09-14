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
			if (UI::Button("Reload")) Scripts::Reload();
			UI::Tooltip("Re-scan the UserScripts folder and re-import every script (edits take effect\n"
						"without a relaunch). Previously-registered script event handlers are cleared\n"
						"first, so reloading doesn't stack duplicates.");

			UI::SeparatorText("Loaded Scripts");

			if (Scripts::scriptList.empty())
			{
				UI::TextDisabled("No scripts found. Drop a .py with a main() into the UserScripts folder, then Reload.");
				return;
			}

			std::string toRun;
			UI::BeginChild("ScriptList", 0, 200);
			for (const auto& script : Scripts::scriptList)
			{
				UI::PushID(script.c_str());
				if (UI::SmallButton("Run")) toRun = script;
				UI::SameLine();
				UI::Text("%s", script.c_str());
				UI::PopID();
			}
			UI::EndChild();
			if (!toRun.empty()) Scripts::ExecuteUnloaded(toRun);
		}
	} // namespace Sections
} // namespace Menu
