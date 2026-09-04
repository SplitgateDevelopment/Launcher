#pragma once

/// @file
/// @brief Misc tab: player FOV/speed sliders, load-into-map / summon-bot actions, overlay/program
/// controls, and the announce-toggles switch. (User scripts moved to the Scripts tab.)

#include <string>
#include <vector>

#include "../../settings/Settings.h"
#include "../../scripting/Events.h"
#include "../../utils/Logger.h" // Logger::SetConsoleVisibility
#include "../gui/Window.h"		// Window::SetStreamproof

// Forward-declared instead of including hook/Hook.h: that header transitively includes this menu
// (via Features -> GUI -> Menu), so including it here would be circular. The inline definition in
// Hook.h is compiled in the same translation unit.
namespace Hook
{
	void RequestUnload();
}

namespace Menu
{
	namespace Sections
	{
		/// @brief Renders the Misc tab.
		/// Adjusts player FOV and (only while in-game) player speed; offers a "Load into map" button that
		/// dispatches LoadIntoMap when not already in-game; toggles user scripts (dispatching SettingsChanged)
		/// and lists the currently loaded scripts.
		void MiscTab()
		{
			bool isInGame = Globals::PlayerController->IsInGame();

			ImGui::SeparatorText("Player");

			ImGui::SetNextItemWidth(180.f);
			ImGui::SliderFloat("##fov", &Settings.EXPLOITS.FOV, 80.0f, 160.0f, "FOV %.0f");
			ImGui::SameLine();
			if (ImGui::SmallButton("Reset##fov"))
			{
				Settings.EXPLOITS.FOV = ExploitsSettings{}.FOV;
				Events::Dispatch(Events::Type::SettingsChanged);
			}

			if (!isInGame) ImGui::BeginDisabled();
			ImGui::SetNextItemWidth(180.f);
			ImGui::SliderFloat("##speed", &Settings.EXPLOITS.PlayerSpeed, 0.2f, 4.f, "Speed %.2f");
			ImGui::SameLine();
			if (ImGui::SmallButton("Reset##speed"))
			{
				Settings.EXPLOITS.PlayerSpeed = ExploitsSettings{}.PlayerSpeed;
				Events::Dispatch(Events::Type::SettingsChanged);
			}
			if (!isInGame) ImGui::EndDisabled();

			ImGui::SeparatorText("Game");

			// "Load into map" is only usable out of a game, and only once per session — it disables
			// itself after the first use.
			static bool loadUsed = false;
			const bool loadDisabled = isInGame || loadUsed;
			if (loadDisabled) ImGui::BeginDisabled();
			if (ImGui::Button("Load into map"))
			{
				Events::Dispatch(Events::Type::LoadIntoMap);
				loadUsed = true;
			};
			if (loadDisabled) ImGui::EndDisabled();

			// Spawn picker: a searchable dropdown of spawnable actor classes (bots, pawns, guns, ...)
			// scanned from GObjects, plus a Spawn button that spawns the selection in front of you.
			{
				static std::vector<std::string> spawnClasses;
				static std::vector<int> filtered;			 // indices into spawnClasses matching the search
				static char search[128] = "";
				static std::string lastSearch = "\x01";		 // sentinel: forces the first filter build
				static std::string selected;
				static bool scanned = false;

				const auto scan = []
				{
					spawnClasses.clear();
					static const char* keywords[] = {"Bot", "Pawn", "Gun", "Weapon", "Character", "Projectile", "Grenade", "Vehicle"};

					// Identify class objects by their class pointer (the Class / BlueprintGeneratedClass
					// meta-class) so GetFullName() — which allocates and walks the outer chain — is only
					// paid on the handful of class objects, not on every one of the ~100k GObjects.
					UObject* classMeta = ObjObjects->FindObject("Class CoreUObject.Class");
					UObject* bgcMeta = ObjObjects->FindObject("Class Engine.BlueprintGeneratedClass");
					const bool fast = (classMeta || bgcMeta);

					const auto count = ObjObjects->NumElements;
					for (auto i = 0u; i < count; i++)
					{
						auto* obj = ObjObjects->GetObjectPtr(i);
						if (!obj) continue;
						auto* cls = reinterpret_cast<UObject*>(obj->ClassPrivate);
						if (fast && cls != classMeta && cls != bgcMeta) continue;

						std::string full = obj->GetFullName();
						if (!fast && full.rfind("Class ", 0) != 0 && full.rfind("BlueprintGeneratedClass ", 0) != 0) continue;

						for (const char* kw : keywords)
							if (full.find(kw) != std::string::npos)
							{
								spawnClasses.push_back(std::move(full));
								break;
							}
					}
					scanned = true;
					lastSearch = "\x01"; // re-filter against the fresh list
				};

				ImGui::SetNextItemWidth(240.f);
				if (ImGui::BeginCombo("##spawnclass", selected.empty() ? "Spawn class..." : selected.c_str()))
				{
					if (!scanned) scan(); // once, not every frame the combo is open

					ImGui::SetNextItemWidth(-1.f);
					ImGui::InputTextWithHint("##spawnsearch", "filter: bot, gun, pawn...", search, sizeof(search));

					// Rebuild the filtered index list only when the search text changes.
					if (search != lastSearch)
					{
						lastSearch = search;
						filtered.clear();
						for (int i = 0; i < static_cast<int>(spawnClasses.size()); i++)
							if (lastSearch.empty() || spawnClasses[i].find(lastSearch) != std::string::npos)
								filtered.push_back(i);
					}

					// Clip to the visible rows so a few-thousand-class list isn't laid out in full each frame.
					ImGui::BeginChild("##spawnlist", ImVec2(340, 220));
					ImGuiListClipper clipper;
					clipper.Begin(static_cast<int>(filtered.size()));
					while (clipper.Step())
						for (int r = clipper.DisplayStart; r < clipper.DisplayEnd; r++)
						{
							const std::string& name = spawnClasses[filtered[r]];
							if (ImGui::Selectable(name.c_str(), name == selected)) selected = name;
						}
					ImGui::EndChild();
					ImGui::EndCombo();
				}
				ImGui::SameLine();
				if (ImGui::SmallButton("Refresh##spawn")) scan();

				ImGui::SameLine();
				if (!isInGame || selected.empty()) ImGui::BeginDisabled();
				if (ImGui::Button("Spawn") && isInGame && !selected.empty() && Globals::PlayerController)
				{
					UObject* cls = ObjObjects->FindObject(selected.c_str());
					auto* pawn = Globals::PlayerController->AcknowledgedPawn;
					if (cls && pawn)
					{
						FVector loc = reinterpret_cast<AActor*>(pawn)->K2_GetActorLocation();
						loc.X += 200.f; // a bit in front of the player
						AActor* actor = SpawnActor(reinterpret_cast<UObject*>(Globals::PlayerController), reinterpret_cast<UClass*>(cls),
												   loc, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn, nullptr);
						Logger::Log(actor ? "SUCCESS" : "ERROR", (actor ? "Spawned " : "Spawn failed: ") + selected);
					}
					else
						Logger::Log("ERROR", "Spawn: class not found: " + selected);
				}
				if (!isInGame || selected.empty()) ImGui::EndDisabled();
				ImGui::Tooltip("Pick a spawnable class (searchable — bots, pawns, guns, ...) and Spawn it in front\nof you. Refresh rescans GObjects. Uses the deferred SpawnActor.");
			}

			ImGui::SeparatorText("Overlay");
			ImGui::Tooltip("Hide the overlay from screen capture (OBS, Discord, Game Bar). Needs Windows 10 2004+.");
			if (ImGui::ToggleButton("Streamproof", &Settings.MENU.Streamproof))
			{
				Window::SetStreamproof(Settings.MENU.Streamproof);
				Events::Dispatch(Events::Type::SettingsChanged);
			}

			ImGui::SeparatorText("Program");
			if (ImGui::Button("Toggle Console"))
			{
				Settings.MISC.ShowConsole = !Settings.MISC.ShowConsole;
				Logger::SetConsoleVisibility(Settings.MISC.ShowConsole);
			}
			ImGui::SameLine();
			if (ImGui::Button("Unload")) Hook::RequestUnload();

			if (ImGui::ToggleButton("Announce toggles in chat", &Settings.MISC.AnnounceToggles))
				Events::Dispatch(Events::Type::SettingsChanged);
			ImGui::Tooltip("Post a local (client-only) chat line when you toggle a feature, e.g. \"[ESP] Enabled\".\nShown only to you, not sent to the server.");

		}
	} // namespace Sections
} // namespace Menu