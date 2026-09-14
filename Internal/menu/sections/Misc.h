#pragma once

/// @file
/// @brief Misc tab: player FOV/speed sliders, load-into-map / summon-bot actions, overlay/program
/// controls, and the announce-toggles switch. (User scripts moved to the Scripts tab.)

#include <string>
#include <vector>

#include "../../settings/Settings.h"
#include "../../scripting/Events.h"
#include "../../cache/ClassCache.h" // shared class list for the spawn picker
#include "../../utils/Logger.h"		// Logger::SetConsoleVisibility
#include "../ui/UI.h"

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
			// Read the cached flag (refreshed by PostRender on the game thread) instead of calling
			// IsInGame() on the live controller: this tab is rendered on the external overlay's own
			// thread, and a map load can free the controller mid-frame — a use-after-free a null check
			// can't catch. Everything below already gates on isInGame.
			bool isInGame = Engine::IsInGame;

			UI::SeparatorText("Player");

			UI::SetNextItemWidth(180.f);
			UI::SliderFloat("##fov", &Settings.EXPLOITS.FOV, 80.0f, 160.0f, "FOV %.0f");
			UI::SameLine();
			if (UI::SmallButton("Reset##fov"))
			{
				Settings.EXPLOITS.FOV = ExploitsSettings{}.FOV;
				Events::Dispatch(Events::Type::SettingsChanged);
			}

			UI::BeginDisabled(!isInGame);
			UI::SetNextItemWidth(180.f);
			UI::SliderFloat("##speed", &Settings.EXPLOITS.PlayerSpeed, 0.2f, 4.f, "Speed %.2f");
			UI::SameLine();
			if (UI::SmallButton("Reset##speed"))
			{
				Settings.EXPLOITS.PlayerSpeed = ExploitsSettings{}.PlayerSpeed;
				Events::Dispatch(Events::Type::SettingsChanged);
			}
			UI::EndDisabled();

			UI::SeparatorText("Game");

			// "Load into map" is usable whenever you're out of a game (e.g. back in the menu after a
			// match), disabled only while already in one. The dropdown beside it picks the target level
			// SwitchLevel travels to. These are the game's Content/Maps package names (leaf, no path or
			// .BuiltData); Simulation_Alpha (the firing range) is index 0 and the default. The selection
			// rides in on the event payload. Enumerated from the FName pool via the SDK tab.
			static const char* const levels[] = {
				// Simulation / firing-range maps
				"Simulation_Alpha", // default
				"Simulation_Bravo",
				"Simulation_Charlie",
				"Simulation_Delta",
				"Simulation_Echo",
				"Simulation_Foxtrot",
				"Simulation_Golf",
				"Simulation_Hotel",
				"Simulation_India",
				"Simulation_Juliet",
				// Arena maps
				"Abyss",
				"Atlantis",
				"Crag",
				"Foregone_Destruction",
				"Helix",
				"Highwind",
				"Impact",
				"Karman_Station",
				"Lavawell",
				"Oasis",
				"Olympus",
				"Pantheon",
				"Silo",
				"Stadium",
				// Special / system maps
				"MainMenu",
				"Lobby",
				"Tutorial",
				"PracticeRange",
				"TravelMap",
				"Forge_Island",
				"Forge_Flat_Earth",
				"Abyss_Cinematics",
				// Blockout / work-in-progress maps
				"Maya_Blockout",
				"Noboru_Temple_Blockout",
				"Decay_Blockout_WIP",
				"Drift_Blockout_WIP",
				"Titan_Blockout_WIP",
				"Toxic_Blockout_Wip",
				"Vessel_Blockout_WIP",
				"Vintage_Blockout_WIP",
			};
			// Searchable dropdown (same pattern as the spawn picker): filter the static list, click a
			// row to select. selectedLevel points into the static array above, so it stays valid to hand
			// to the event payload.
			static std::vector<int> levelFiltered;
			static char levelSearch[128] = "";
			static std::string levelLastKey = "\x01"; // sentinel: forces the first filter build
			static const char* selectedLevel = levels[0];

			UI::BeginDisabled(isInGame);
			UI::SetNextItemWidth(220.f);
			if (UI::BeginCombo("##level", selectedLevel))
			{
				UI::SetNextItemWidth(-1.f);
				UI::InputTextHint("##levelsearch", "filter maps...", levelSearch, sizeof(levelSearch));

				// Rebuild the filtered index list only when the search text changes.
				if (levelSearch != levelLastKey)
				{
					levelLastKey = levelSearch;
					levelFiltered.clear();
					const std::string needle = levelSearch;
					for (int i = 0; i < UI::Count(levels); i++)
						if (needle.empty() || std::string(levels[i]).find(needle) != std::string::npos)
							levelFiltered.push_back(i);
				}

				UI::BeginChild("##levellist", 240, 200);
				UI::ClippedList(static_cast<int>(levelFiltered.size()), [&](int r) {
						const char* name = levels[levelFiltered[r]];
						if (UI::Selectable(name, name == selectedLevel)) selectedLevel = name;
					});
				UI::EndChild();
				UI::EndCombo();
			}
			UI::SameLine();
			if (UI::Button("Load into map"))
			{
				Events::Payload payload;
				payload.name = selectedLevel;
				Events::Dispatch(Events::Type::LoadIntoMap, payload);
			}
			UI::EndDisabled();

			UI::SameLine();
			UI::BeginDisabled(!isInGame);
			if (UI::Button("Respawn"))
				if (auto* character = reinterpret_cast<APortalWarsCharacter*>(Engine::PlayerController->Character))
					character->RequestSuicide();
			UI::EndDisabled();
			UI::Tooltip("Kill your character so it respawns (RequestSuicide).");

			// Spawn picker: a searchable dropdown of spawnable actor classes (bots, pawns, guns, ...)
			// scanned from GObjects, plus a Spawn button that spawns the selection in front of you.
			{
				static std::vector<int> filtered; // indices into the shared ClassCache
				static char search[128] = "";
				static std::string lastKey = "\x01";	// sentinel: forces the first filter build
				static size_t lastCacheSize = SIZE_MAX; // re-filter when the cache is rebuilt
				static std::string selected;
				static const char* keywords[] = {"Bot", "Pawn", "Gun", "Weapon", "Character", "Projectile", "Grenade", "Vehicle"};

				UI::SetNextItemWidth(240.f);
				if (UI::BeginCombo("##spawnclass", selected.empty() ? "Spawn class..." : selected.c_str()))
				{
					const auto& classes = ClassCache::Get(); // shared, built once

					UI::SetNextItemWidth(-1.f);
					UI::InputTextHint("##spawnsearch", "filter: bot, gun, pawn...", search, sizeof(search));

					// Rebuild the filtered index list only when the search or the underlying cache changes.
					if (search != lastKey || classes.size() != lastCacheSize)
					{
						lastKey = search;
						lastCacheSize = classes.size();
						filtered.clear();
						const std::string needle = search;
						for (int i = 0; i < static_cast<int>(classes.size()); i++)
						{
							const std::string& name = classes[i].name;
							bool spawnable = false; // narrow to bots/pawns/guns/... so it's a spawn list, not every class
							for (const char* kw : keywords)
								if (name.find(kw) != std::string::npos)
								{
									spawnable = true;
									break;
								}
							if (!spawnable) continue;
							if (!needle.empty() && name.find(needle) == std::string::npos) continue;
							filtered.push_back(i);
						}
					}

					// Clip to the visible rows so a few-thousand-class list isn't laid out in full each frame.
					UI::BeginChild("##spawnlist", 340, 220);
					UI::ClippedList(static_cast<int>(filtered.size()), [&](int r) {
							const std::string& name = classes[filtered[r]].name;
							if (UI::Selectable(name.c_str(), name == selected)) selected = name;
						});
					UI::EndChild();
					UI::EndCombo();
				}
				UI::SameLine();
				if (UI::SmallButton("Refresh##spawn")) ClassCache::Rebuild();

				UI::SameLine();
				UI::BeginDisabled(!isInGame || selected.empty());
				if (UI::Button("Spawn") && isInGame && !selected.empty() && Engine::PlayerController)
				{
					UObject* cls = Engine::GObjects->FindObject(selected.c_str());
					auto* pawn = Engine::PlayerController->AcknowledgedPawn;
					if (cls && pawn)
					{
						FVector loc = reinterpret_cast<AActor*>(pawn)->K2_GetActorLocation();
						loc.X += 200.f; // a bit in front of the player
						AActor* actor = SpawnActor(reinterpret_cast<UObject*>(Engine::PlayerController), reinterpret_cast<UClass*>(cls),
												   loc, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn, nullptr);
						Logger::Log(actor ? "SUCCESS" : "ERROR", (actor ? "Spawned " : "Spawn failed: ") + selected);
					}
					else
						Logger::Log("ERROR", "Spawn: class not found: " + selected);
				}
				UI::EndDisabled();
				UI::Tooltip("Pick a spawnable class (searchable — bots, pawns, guns, ...) and Spawn it in front\nof you. Refresh rescans GObjects. Uses the deferred SpawnActor.");
			}

			UI::SeparatorText("Cosmetics");
			{
				static std::vector<int> skinFiltered; // indices into the shared ClassCache
				static char skinSearch[128] = "";
				static std::string skinLastKey = "\x01";
				static size_t skinLastCacheSize = SIZE_MAX;
				static std::string skinSelected;

				UI::SetNextItemWidth(240.f);
				if (UI::BeginCombo("##skinclass", skinSelected.empty() ? "Skin class..." : skinSelected.c_str()))
				{
					const auto& classes = ClassCache::Get();

					UI::SetNextItemWidth(-1.f);
					UI::InputTextHint("##skinsearch", "filter: skin name...", skinSearch, sizeof(skinSearch));

					if (skinSearch != skinLastKey || classes.size() != skinLastCacheSize)
					{
						skinLastKey = skinSearch;
						skinLastCacheSize = classes.size();
						skinFiltered.clear();
						const std::string needle = skinSearch;
						for (int i = 0; i < static_cast<int>(classes.size()); i++)
						{
							const std::string& name = classes[i].name;
							if (name.find("Skin") == std::string::npos) continue; // skins only
							if (!needle.empty() && name.find(needle) == std::string::npos) continue;
							skinFiltered.push_back(i);
						}
					}

					UI::BeginChild("##skinlist", 340, 220);
					UI::ClippedList(static_cast<int>(skinFiltered.size()), [&](int r) {
							const std::string& name = classes[skinFiltered[r]].name;
							if (UI::Selectable(name.c_str(), name == skinSelected)) skinSelected = name;
						});
					UI::EndChild();
					UI::EndCombo();
				}
				UI::SameLine();
				if (UI::SmallButton("Refresh##skin")) ClassCache::Rebuild();

				UI::SameLine();
				UI::BeginDisabled(!isInGame || skinSelected.empty());
				if (UI::Button("Apply skin") && isInGame && !skinSelected.empty() && Engine::PlayerController)
				{
					UObject* cls = Engine::GObjects->FindObject(skinSelected.c_str());
					auto* character = reinterpret_cast<APortalWarsCharacter*>(Engine::PlayerController->Character);
					if (cls && character)
					{
						character->CharacterSkinClass = reinterpret_cast<ACharacterSkin*>(cls);
						character->UpdateSkins();
						Logger::Log("SUCCESS", "Applied skin: " + skinSelected);
					}
					else
						Logger::Log("ERROR", "Apply skin: class not found: " + skinSelected);
				}
				UI::EndDisabled();
				UI::Tooltip("Pick a skin class, then apply it to your character, gun or jetpack.\nClient-side (sets the *SkinClass + UpdateSkins); the server may re-assert your real skins. Refresh rescans classes.");

				// Apply the selected class to the gun / jetpack too (they use their own skin types).
				UI::BeginDisabled(!isInGame || skinSelected.empty());
				if (UI::Button("Apply gun skin") && isInGame && !skinSelected.empty() && Engine::PlayerController)
				{
					UObject* cls = Engine::GObjects->FindObject(skinSelected.c_str());
					auto* character = reinterpret_cast<APortalWarsCharacter*>(Engine::PlayerController->Character);
					if (cls && character && character->CurrentWeapon)
					{
						character->CurrentWeapon->WeaponSkinClass = reinterpret_cast<ABaseGunSkin*>(cls);
						character->CurrentWeapon->UpdateSkins();
						Logger::Log("SUCCESS", "Applied gun skin: " + skinSelected);
					}
				}
				UI::SameLine();
				if (UI::Button("Apply jetpack skin") && isInGame && !skinSelected.empty() && Engine::PlayerController)
				{
					UObject* cls = Engine::GObjects->FindObject(skinSelected.c_str());
					auto* character = reinterpret_cast<APortalWarsCharacter*>(Engine::PlayerController->Character);
					if (cls && character)
					{
						character->JetpackSkinClass = reinterpret_cast<AJetpackSkin*>(cls);
						character->UpdateSkins();
						Logger::Log("SUCCESS", "Applied jetpack skin: " + skinSelected);
					}
				}
				UI::EndDisabled();
			}


			UI::SeparatorText("Program");
			if (UI::Button("Toggle Console"))
			{
				Settings.MISC.ShowConsole = !Settings.MISC.ShowConsole;
				Logger::SetConsoleVisibility(Settings.MISC.ShowConsole);
			}
			UI::SameLine();
			if (UI::Button("Unload")) Hook::RequestUnload();

			if (UI::Toggle("Announce toggles in chat", &Settings.MISC.AnnounceToggles))
				Events::Dispatch(Events::Type::SettingsChanged);
			UI::Tooltip("Post a local (client-only) chat line when you toggle a feature, e.g. \"[ESP] Enabled\".\nShown only to you, not sent to the server.");
		}
	} // namespace Sections
} // namespace Menu