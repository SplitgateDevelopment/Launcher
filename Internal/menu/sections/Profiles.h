#pragma once

/// @file
/// @brief Profiles tab: save/load/delete named settings profiles, and export/import share codes.

#include <cstring>

#include "../../settings/Profiles.h"

namespace Menu
{
	namespace Sections
	{
		/// @brief Renders the Profiles tab: a save-as field, the list of saved profiles (load/delete),
		/// and share-code copy/paste-import via the clipboard.
		void ProfilesTab()
		{
			ImGui::SeparatorText("Save current config");
			static char nameBuffer[64] = "";
			ImGui::SetNextItemWidth(200.f);
			ImGui::InputText("##name", nameBuffer, sizeof(nameBuffer));
			ImGui::SameLine();
			if (ImGui::Button("Save") && nameBuffer[0])
			{
				Profiles::Save(nameBuffer);
				nameBuffer[0] = '\0';
			}

			ImGui::SeparatorText("Profiles");
			std::string toLoad, toDelete;
			for (const auto& name : Profiles::List())
			{
				ImGui::PushID(name.c_str());
				if (ImGui::SmallButton("Load")) toLoad = name;
				ImGui::SameLine();
				if (ImGui::SmallButton("Delete")) toDelete = name;
				ImGui::SameLine();
				ImGui::TextUnformatted(name.c_str());
				ImGui::PopID();
			}
			if (!toLoad.empty()) Profiles::Load(toLoad);
			if (!toDelete.empty()) Profiles::Delete(toDelete);

			ImGui::SeparatorText("Share code");
			ImGui::Tooltip("A portable code for the current config. Copy to share; paste + import to apply.");
			if (ImGui::Button("Copy current config")) ImGui::SetClipboardText(Profiles::Export().c_str());

			static char codeBuffer[8192] = "";
			ImGui::InputTextMultiline("##code", codeBuffer, sizeof(codeBuffer), ImVec2(0, 80));

			if (ImGui::Button("Paste"))
			{
				const char* clip = ImGui::GetClipboardText();
				if (clip) strncpy_s(codeBuffer, clip, _TRUNCATE);
			}
			ImGui::SameLine();
			if (ImGui::Button("Import") && codeBuffer[0])
			{
				if (Profiles::Import(codeBuffer)) codeBuffer[0] = '\0';
			}
		}
	} // namespace Sections
} // namespace Menu
