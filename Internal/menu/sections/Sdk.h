#pragma once

/// @file
/// @brief SDK tab: a live UObject explorer (search object names, list a class's instances) plus the
/// GObjects dump-to-file. All reads go through the already-resolved `Engine::GObjects` (TUObjectArray),
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
#include "../../cache/ClassCache.h"
#include "../../ue/Engine.h"
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
				const auto count = Engine::GObjects->NumElements;
				for (auto i = 0u; i < count; i++)
				{
					auto* obj = Engine::GObjects->GetObjectPtr(i);
					if (!obj) continue;
					std::string full = obj->GetFullName();
					if (full.find(needle) == std::string::npos) continue;
					nameTotal++;
					if (nameResults.size() < 1000)
						nameResults.push_back({static_cast<int>(obj->InternalIndex), std::move(full), reinterpret_cast<std::uintptr_t>(obj)});
				}
				Logger::Log("INFO", std::format("[SDK] {} objects match \"{}\" ({} shown)", nameTotal, needle, nameResults.size()));
			}
			ImGui::SameLine();
			if (ImGui::Button("Copy##names"))
			{
				std::string out;
				for (const auto& row : nameResults) out += std::format("[{}] {}\n", row.index, row.name);
				ImGui::SetClipboardText(out.c_str());
			}
			ImGui::Tooltip("Copy the listed results to the clipboard.");

			if (!nameResults.empty())
			{
				ImGui::Text("%d match(es)%s", nameTotal, nameTotal > (int)nameResults.size() ? " (first 1000)" : "");
				ImGui::BeginChild("NameResults", ImVec2(0, 180), true, ImGuiWindowFlags_HorizontalScrollbar);
				// Only lay out the rows actually on screen (a fixed-height Text list), so a 1000-row
				// result doesn't cost 1000 widgets every frame.
				ImGuiListClipper clipper;
				clipper.Begin(static_cast<int>(nameResults.size()));
				while (clipper.Step())
					for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
						ImGui::Text("[%d] %s", nameResults[i].index, nameResults[i].name.c_str());
				ImGui::EndChild();
			}

			ImGui::SeparatorText("Class search");
			ImGui::Tooltip("Filter the cached list of every class (shared with the Misc spawn picker; built\nonce, Refresh to rebuild). Click a row to copy the exact name.");

			static char classFilter[128] = "";
			static std::vector<int> classFiltered;
			static std::string lastClassKey = "\x01";	 // sentinel: forces the first filter build
			static size_t lastClassCacheSize = SIZE_MAX; // re-filter when the cache is rebuilt
			ImGui::SetNextItemWidth(260.f);
			ImGui::InputText("##classfilter", classFilter, sizeof(classFilter));
			ImGui::SameLine();
			if (ImGui::Button("Refresh##classes")) ClassCache::Rebuild();
			ImGui::SameLine();
			if (ImGui::Button("Copy##classes"))
			{
				std::string out;
				const std::string needle = classFilter;
				for (const auto& entry : ClassCache::Get())
					if (needle.empty() || entry.name.find(needle) != std::string::npos) out += entry.name + "\n";
				ImGui::SetClipboardText(out.c_str());
			}
			ImGui::Tooltip("Copy the filtered class names to the clipboard.");
			{
				const auto& classes = ClassCache::Get();
				if (classFilter != lastClassKey || classes.size() != lastClassCacheSize)
				{
					lastClassKey = classFilter;
					lastClassCacheSize = classes.size();
					classFiltered.clear();
					const std::string needle = classFilter;
					for (int i = 0; i < static_cast<int>(classes.size()); i++)
						if (needle.empty() || classes[i].name.find(needle) != std::string::npos)
							classFiltered.push_back(i);
				}

				ImGui::Text("%d / %d classes", static_cast<int>(classFiltered.size()), static_cast<int>(classes.size()));
				ImGui::BeginChild("ClassResults", ImVec2(0, 160), true, ImGuiWindowFlags_HorizontalScrollbar);
				ImGuiListClipper clipper;
				clipper.Begin(static_cast<int>(classFiltered.size()));
				while (clipper.Step())
					for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
					{
						const std::string& name = classes[classFiltered[i]].name;
						if (ImGui::Selectable(name.c_str())) ImGui::SetClipboardText(name.c_str());
					}
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
				UObject* classObj = Engine::GObjects->FindObject(className);
				if (!classObj) classObj = Engine::GObjects->FindObject((std::string("Class ") + className).c_str());
				if (!classObj) classObj = Engine::GObjects->FindObject((std::string("BlueprintGeneratedClass ") + className).c_str());

				if (!classObj)
				{
					classStatus = std::format("class \"{}\" not found", className);
				}
				else
				{
					const auto count = Engine::GObjects->NumElements;
					for (auto i = 0u; i < count; i++)
					{
						auto* obj = Engine::GObjects->GetObjectPtr(i);
						if (!obj || !obj->IsA(classObj)) continue;
						instanceTotal++;
						if (instanceResults.size() < 1000)
							instanceResults.push_back({static_cast<int>(obj->InternalIndex), obj->GetFullName(), reinterpret_cast<std::uintptr_t>(obj)});
					}
					classStatus = std::format("{}: {} instance(s)", classObj->GetFullName(), instanceTotal);
				}
				Logger::Log("INFO", "[SDK] " + classStatus);
			}
			ImGui::SameLine();
			if (ImGui::Button("Copy##instances"))
			{
				std::string out;
				for (const auto& row : instanceResults) out += std::format("[{}] 0x{:x} {}\n", row.index, row.address, row.name);
				ImGui::SetClipboardText(out.c_str());
			}
			ImGui::Tooltip("Copy the listed instances (index, address, name) to the clipboard.");
			if (!classStatus.empty()) ImGui::TextUnformatted(classStatus.c_str());
			if (!instanceResults.empty())
			{
				ImGui::BeginChild("InstanceResults", ImVec2(0, 180), true, ImGuiWindowFlags_HorizontalScrollbar);
				ImGuiListClipper clipper;
				clipper.Begin(static_cast<int>(instanceResults.size()));
				while (clipper.Step())
					for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
						ImGui::Text("[%d] 0x%llx %s", instanceResults[i].index, static_cast<unsigned long long>(instanceResults[i].address), instanceResults[i].name.c_str());
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

				auto objNum = Engine::GObjects->NumElements;
				for (auto i = 0u; i < objNum; i++)
				{
					auto Object = Engine::GObjects->GetObjectPtr(i);
					if (!Object) continue;
					file << '[' + std::to_string(Object->InternalIndex) + "] " + Object->GetFullName() << '\n';
				}
				file.close();

				std::string msg = std::format("Dumped {} GObjects to {}", objNum, filePath.string());
				Logger::Log("SUCCESS", msg);
				if (Engine::PlayerController) Engine::PlayerController->SendChatMessage(FString(msg));
			}
			ImGui::Tooltip("Write every GObject (index + full name) to Dumps/GObjects.txt.");
		}
	} // namespace Sections
} // namespace Menu
