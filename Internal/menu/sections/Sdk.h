#pragma once

/// @file
/// @brief SDK tab: a live UObject explorer (search object names, list a class's instances), a
/// searchable FName-pool enumeration (via `Engine::GNames`, covering names for not-yet-loaded
/// content such as map/level names), plus the GObjects dump-to-file. Object/class reads go through
/// the already-resolved `Engine::GObjects` (TUObjectArray), `FindObject`, `IsA`, and `GetFullName` —
/// no new offsets. Scans run on demand (button press), not per frame, since a full walk is the same
/// cost as Dump GObjects.

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <format>
#include <fstream>
#include <regex>
#include <string>
#include <vector>

#include "../../settings/Settings.h"
#include "../../cache/ClassCache.h"
#include "../../cache/NameCache.h"
#include "../../ue/Engine.h"
#include "../../utils/Logger.h"
#include "../ui/UI.h"

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

			UI::SeparatorText("Object search");
			UI::Tooltip("Scan every GObject and list those whose full name contains the text.\nOn demand (a full walk, like Dump GObjects).");

			static char nameFilter[128] = "";
			static std::vector<SdkRow> nameResults;
			static int nameTotal = 0;
			UI::SetNextItemWidth(260.f);
			UI::InputText("##namefilter", nameFilter, sizeof(nameFilter));
			UI::SameLine();
			if (UI::Button("Search names") && nameFilter[0])
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
			UI::SameLine();
			if (UI::Button("Copy##names"))
			{
				std::string out;
				for (const auto& row : nameResults)
					out += std::format("[{}] {}\n", row.index, row.name);
				UI::SetClipboardText(out.c_str());
			}
			UI::Tooltip("Copy the listed results to the clipboard.");

			if (!nameResults.empty())
			{
				UI::Text("%d match(es)%s", nameTotal, nameTotal > (int)nameResults.size() ? " (first 1000)" : "");
				UI::BeginChild("NameResults", 0, 180);
				UI::ClippedList(static_cast<int>(nameResults.size()), [&](int i)
							   { UI::Text("[%d] %s", nameResults[i].index, nameResults[i].name.c_str()); });
				UI::EndChild();
			}

			UI::SeparatorText("Class search");
			UI::Tooltip("Filter the cached list of every class (shared with the Misc spawn picker; built\nonce, Refresh to rebuild). Click a row to copy the exact name.");

			static char classFilter[128] = "";
			static std::vector<int> classFiltered;
			static std::string lastClassKey = "\x01";	 // sentinel: forces the first filter build
			static size_t lastClassCacheSize = SIZE_MAX; // re-filter when the cache is rebuilt
			UI::SetNextItemWidth(260.f);
			UI::InputText("##classfilter", classFilter, sizeof(classFilter));
			UI::SameLine();
			if (UI::Button("Refresh##classes")) ClassCache::Rebuild();
			UI::SameLine();
			if (UI::Button("Copy##classes"))
			{
				std::string out;
				const std::string needle = classFilter;
				for (const auto& entry : ClassCache::Get())
					if (needle.empty() || entry.name.find(needle) != std::string::npos) out += entry.name + "\n";
				UI::SetClipboardText(out.c_str());
			}
			UI::Tooltip("Copy the filtered class names to the clipboard.");
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

				UI::Text("%d / %d classes", static_cast<int>(classFiltered.size()), static_cast<int>(classes.size()));
				UI::BeginChild("ClassResults", 0, 160);
				UI::ClippedList(static_cast<int>(classFiltered.size()), [&](int i)
							   {
						const std::string& name = classes[classFiltered[i]].name;
						if (UI::Selectable(name.c_str())) UI::SetClipboardText(name.c_str());
					});
				UI::EndChild();
			}

			UI::SeparatorText("Name pool (FNames)");
			UI::Tooltip("Search every interned FName, including names for content that isn't loaded\n(e.g. map/level names to travel to). Built once, Refresh to rebuild. Click a row to copy.");
			{
				static char nameFilter2[128] = "";
				static bool caseSensitive = false; // default: case-insensitive
				static bool useRegex = false;	   // plain substring by default
				static std::vector<int> nameFiltered;
				static std::string regexError;				// last regex compile error, shown when useRegex
				static std::string lastNameKey = "\x01";	// sentinel: forces the first filter build
				static size_t lastNameCacheSize = SIZE_MAX; // re-filter when the cache is rebuilt
				static bool lastCase = false, lastRegex = false;

				UI::SetNextItemWidth(260.f);
				UI::InputText("##namepoolfilter", nameFilter2, sizeof(nameFilter2));
				UI::SameLine();
				if (UI::Button("Refresh##names")) NameCache::Rebuild();

				UI::Checkbox("Case sensitive##names", &caseSensitive);
				UI::SameLine();
				UI::Checkbox("Regex##names", &useRegex);
				UI::Tooltip("ECMAScript regex, matched as a search (unanchored, so a bare pattern behaves like\n\"contains\"). Anchor with ^ and $ to constrain: ^/Game/Maps/[^/]+$ matches a map\npackage but not the assets nested under it.");

				// Re-filter only when an input changes (text, cache, or a toggle), not every frame.
				const auto& allNames = NameCache::Get();
				if (nameFilter2 != lastNameKey || allNames.size() != lastNameCacheSize || caseSensitive != lastCase || useRegex != lastRegex)
				{
					lastNameKey = nameFilter2;
					lastNameCacheSize = allNames.size();
					lastCase = caseSensitive;
					lastRegex = useRegex;
					nameFiltered.clear();
					regexError.clear();

					const std::string needle = nameFilter2;
					if (needle.empty())
					{
						for (int i = 0; i < static_cast<int>(allNames.size()); i++)
							nameFiltered.push_back(i);
					}
					else if (useRegex)
					{
						try
						{
							auto flags = std::regex::ECMAScript;
							if (!caseSensitive) flags |= std::regex::icase;
							const std::regex re(needle, flags);
							for (int i = 0; i < static_cast<int>(allNames.size()); i++)
								if (std::regex_search(allNames[i], re)) nameFiltered.push_back(i);
						}
						catch (const std::regex_error& e)
						{
							regexError = e.what(); // invalid pattern: show it, match nothing
						}
					}
					else
					{
						// Case-insensitive substring without allocating a lowercased copy per name.
						auto ci = [](char a, char b)
						{ return std::tolower(static_cast<unsigned char>(a)) == std::tolower(static_cast<unsigned char>(b)); };
						for (int i = 0; i < static_cast<int>(allNames.size()); i++)
						{
							const std::string& name = allNames[i];
							const bool match = caseSensitive
												   ? name.find(needle) != std::string::npos
												   : std::search(name.begin(), name.end(), needle.begin(), needle.end(), ci) != name.end();
							if (match) nameFiltered.push_back(i);
						}
					}
				}

				if (useRegex && !regexError.empty())
					UI::Text("regex error: %s", regexError.c_str());

				if (UI::Button("Copy##namepool"))
				{
					std::string out;
					for (int idx : nameFiltered)
						out += allNames[idx] + "\n";
					UI::SetClipboardText(out.c_str());
				}
				UI::Tooltip("Copy the filtered names to the clipboard.");
				UI::SameLine();
				UI::Text("%d / %d names", static_cast<int>(nameFiltered.size()), static_cast<int>(allNames.size()));

				UI::BeginChild("NamePoolResults", 0, 160);
				UI::ClippedList(static_cast<int>(nameFiltered.size()), [&](int i)
							   {
						const std::string& name = allNames[nameFiltered[i]];
						if (UI::Selectable(name.c_str())) UI::SetClipboardText(name.c_str());
					});
				UI::EndChild();
			}

			UI::SeparatorText("Class instances");
			UI::Tooltip("Resolve a class by name, then list its live instances (IsA).\nAccepts a full name (\"Class PortalWars.PortalWarsCharacter\") or a bare class name.");

			static char className[128] = "";
			static std::vector<SdkRow> instanceResults;
			static int instanceTotal = 0;
			static std::string classStatus;
			UI::SetNextItemWidth(260.f);
			UI::InputText("##classname", className, sizeof(className));
			UI::SameLine();
			if (UI::Button("List instances") && className[0])
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
			UI::SameLine();
			if (UI::Button("Copy##instances"))
			{
				std::string out;
				for (const auto& row : instanceResults)
					out += std::format("[{}] 0x{:x} {}\n", row.index, row.address, row.name);
				UI::SetClipboardText(out.c_str());
			}
			UI::Tooltip("Copy the listed instances (index, address, name) to the clipboard.");
			if (!classStatus.empty()) UI::Text("%s", classStatus.c_str());
			if (!instanceResults.empty())
			{
				UI::BeginChild("InstanceResults", 0, 180);
				UI::ClippedList(static_cast<int>(instanceResults.size()), [&](int i)
							   { UI::Text("[%d] 0x%llx %s", instanceResults[i].index, static_cast<unsigned long long>(instanceResults[i].address), instanceResults[i].name.c_str()); });
				UI::EndChild();
			}

			UI::SeparatorText("Dump");
			if (UI::Button("Dump GObjects"))
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
			UI::Tooltip("Write every GObject (index + full name) to Dumps/GObjects.txt.");

			UI::SameLine();
			if (UI::Button("Dump FName pool"))
			{
				fs::path dumpsDir = Shared::AppDataPath(SettingsHelper::AppFolder) / "Dumps";
				if (!fs::exists(dumpsDir)) fs::create_directories(dumpsDir);

				fs::path filePath = dumpsDir / "FNames.txt";
				std::ofstream file(filePath, std::ios::out | std::ios::trunc);
				if (!file.is_open())
				{
					char errorMsg[256];
					strerror_s(errorMsg, sizeof(errorMsg), errno);
					Logger::Log("ERROR", std::format("Failed to open {} for writing: {}", filePath.string(), errorMsg));
					return;
				}

				const auto& names = NameCache::Get();
				for (const auto& name : names)
					file << name << '\n';
				file.close();

				std::string msg = std::format("Dumped {} FNames to {}", names.size(), filePath.string());
				Logger::Log("SUCCESS", msg);
				if (Engine::PlayerController) Engine::PlayerController->SendChatMessage(FString(msg));
			}
			UI::Tooltip("Write the whole FName pool (every interned name, incl. not-yet-loaded content) to\nDumps/FNames.txt. Uses the cached pool — Refresh the Name pool list above to rescan first.");
		}
	} // namespace Sections
} // namespace Menu
