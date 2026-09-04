#pragma once

/// @file
/// @brief SDK tab: a live UObject explorer (search object names, list a class's instances) plus the
/// GObjects dump-to-file. All reads go through the already-resolved `ObjObjects` (TUObjectArray),
/// `FindObject`, `IsA`, and `GetFullName` — no new offsets. Scans run on demand (button press), not
/// per frame, since a full GObjects walk is the same cost as Dump GObjects.

#include <cstdint>
#include <cstring>
#include <filesystem>
#include <format>
#include <fstream>
#include <string>
#include <vector>

#include "../../settings/Settings.h"
#include "../../utils/Globals.h"
#include "../../utils/Logger.h"

namespace Menu
{
	namespace Sections
	{
		/// One row of a scan result: object index + full name (+ address for instance scans).
		struct SdkRow
		{
			int index;
			std::string name;
			std::uintptr_t address;
		};

		/// @brief Renders the SDK tab.
		void SdkTab()
		{
			ImGui::SeparatorText("Object search");
			ImGui::Tooltip("Scan every GObject and list those whose full name contains the text.\nOn demand (a full walk, like Dump GObjects).");

			static char nameFilter[128] = "";
			static std::vector<SdkRow> nameResults;
			static int nameTotal = 0;
			ImGui::SetNextItemWidth(260.f);
			ImGui::InputText("##namefilter", nameFilter, sizeof(nameFilter));
			ImGui::SameLine();
			if (ImGui::Button("Search names") && nameFilter[0])
			{
				nameResults.clear();
				nameTotal = 0;
				const std::string needle = nameFilter;
				const auto count = ObjObjects->NumElements;
				for (auto i = 0u; i < count; i++)
				{
					auto* obj = ObjObjects->GetObjectPtr(i);
					if (!obj) continue;
					std::string full = obj->GetFullName();
					if (full.find(needle) == std::string::npos) continue;
					nameTotal++;
					if (nameResults.size() < 1000)
						nameResults.push_back({static_cast<int>(obj->InternalIndex), std::move(full), reinterpret_cast<std::uintptr_t>(obj)});
				}
				Logger::Log("INFO", std::format("[SDK] {} objects match \"{}\" ({} shown)", nameTotal, needle, nameResults.size()));
			}

			if (!nameResults.empty())
			{
				ImGui::Text("%d match(es)%s", nameTotal, nameTotal > (int)nameResults.size() ? " (first 1000)" : "");
				ImGui::BeginChild("NameResults", ImVec2(0, 180), true, ImGuiWindowFlags_HorizontalScrollbar);
				for (const auto& row : nameResults)
					ImGui::Text("[%d] %s", row.index, row.name.c_str());
				ImGui::EndChild();
			}

			ImGui::SeparatorText("Class instances");
			ImGui::Tooltip("Resolve a class by name, then list its live instances (IsA).\nAccepts a full name (\"Class PortalWars.PortalWarsCharacter\") or a bare class name.");

			static char className[128] = "";
			static std::vector<SdkRow> instanceResults;
			static int instanceTotal = 0;
			static std::string classStatus;
			ImGui::SetNextItemWidth(260.f);
			ImGui::InputText("##classname", className, sizeof(className));
			ImGui::SameLine();
			if (ImGui::Button("List instances") && className[0])
			{
				instanceResults.clear();
				instanceTotal = 0;

				// Try the given name, then a couple of common "<meta> <name>" forms.
				UObject* classObj = ObjObjects->FindObject(className);
				if (!classObj) classObj = ObjObjects->FindObject((std::string("Class ") + className).c_str());
				if (!classObj) classObj = ObjObjects->FindObject((std::string("BlueprintGeneratedClass ") + className).c_str());

				if (!classObj)
				{
					classStatus = std::format("class \"{}\" not found", className);
				}
				else
				{
					const auto count = ObjObjects->NumElements;
					for (auto i = 0u; i < count; i++)
					{
						auto* obj = ObjObjects->GetObjectPtr(i);
						if (!obj || !obj->IsA(classObj)) continue;
						instanceTotal++;
						if (instanceResults.size() < 1000)
							instanceResults.push_back({static_cast<int>(obj->InternalIndex), obj->GetFullName(), reinterpret_cast<std::uintptr_t>(obj)});
					}
					classStatus = std::format("{}: {} instance(s)", classObj->GetFullName(), instanceTotal);
				}
				Logger::Log("INFO", "[SDK] " + classStatus);
			}
			if (!classStatus.empty()) ImGui::TextUnformatted(classStatus.c_str());
			if (!instanceResults.empty())
			{
				ImGui::BeginChild("InstanceResults", ImVec2(0, 180), true, ImGuiWindowFlags_HorizontalScrollbar);
				for (const auto& row : instanceResults)
					ImGui::Text("[%d] 0x%llx %s", row.index, static_cast<unsigned long long>(row.address), row.name.c_str());
				ImGui::EndChild();
			}

			ImGui::SeparatorText("Dump");
			if (ImGui::Button("Dump GObjects"))
			{
				fs::path dumpsDir = Shared::AppDataPath(SettingsHelper::AppFolder) / "Dumps";
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
				if (Globals::PlayerController) Globals::PlayerController->SendChatMessage(FString(msg));
			}
			ImGui::Tooltip("Write every GObject (index + full name) to Dumps/GObjects.txt.");
		}
	} // namespace Sections
} // namespace Menu
