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
			bool isInGame = Globals::IsInGame;

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

			// "Load into map" is usable whenever you're out of a game (e.g. back in the menu after a
			// match), disabled only while already in one.
			if (isInGame) ImGui::BeginDisabled();
			if (ImGui::Button("Load into map"))
				Events::Dispatch(Events::Type::LoadIntoMap);
			if (isInGame) ImGui::EndDisabled();

			ImGui::SameLine();
			if (!isInGame) ImGui::BeginDisabled();
			if (ImGui::Button("Respawn"))
				if (auto* character = reinterpret_cast<APortalWarsCharacter*>(Globals::PlayerController->Character))
					character->RequestSuicide();
			if (!isInGame) ImGui::EndDisabled();
			ImGui::Tooltip("Kill your character so it respawns (RequestSuicide).");

			// Spawn picker: a searchable dropdown of spawnable actor classes (bots, pawns, guns, ...)
			// scanned from GObjects, plus a Spawn button that spawns the selection in front of you.
			{
				static std::vector<int> filtered;	   // indices into the shared ClassCache
				static char search[128] = "";
				static std::string lastKey = "\x01";   // sentinel: forces the first filter build
				static size_t lastCacheSize = SIZE_MAX; // re-filter when the cache is rebuilt
				static std::string selected;
				static const char* keywords[] = {"Bot", "Pawn", "Gun", "Weapon", "Character", "Projectile", "Grenade", "Vehicle"};

				ImGui::SetNextItemWidth(240.f);
				if (ImGui::BeginCombo("##spawnclass", selected.empty() ? "Spawn class..." : selected.c_str()))
				{
					const auto& classes = ClassCache::Get(); // shared, built once

					ImGui::SetNextItemWidth(-1.f);
					ImGui::InputTextWithHint("##spawnsearch", "filter: bot, gun, pawn...", search, sizeof(search));

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
								if (name.find(kw) != std::string::npos) { spawnable = true; break; }
							if (!spawnable) continue;
							if (!needle.empty() && name.find(needle) == std::string::npos) continue;
							filtered.push_back(i);
						}
					}

					// Clip to the visible rows so a few-thousand-class list isn't laid out in full each frame.
					ImGui::BeginChild("##spawnlist", ImVec2(340, 220));
					ImGuiListClipper clipper;
					clipper.Begin(static_cast<int>(filtered.size()));
					while (clipper.Step())
						for (int r = clipper.DisplayStart; r < clipper.DisplayEnd; r++)
						{
							const std::string& name = classes[filtered[r]].name;
							if (ImGui::Selectable(name.c_str(), name == selected)) selected = name;
						}
					ImGui::EndChild();
					ImGui::EndCombo();
				}
				ImGui::SameLine();
				if (ImGui::SmallButton("Refresh##spawn")) ClassCache::Rebuild();

				ImGui::SameLine();
				if (!isInGame || selected.empty()) ImGui::BeginDisabled();
				if (ImGui::Button("Spawn") && isInGame && !selected.empty() && Globals::PlayerController)
				{
					UObject* cls = Engine::GObjects->FindObject(selected.c_str());
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

			ImGui::SeparatorText("Cosmetics");
			{
				static std::vector<int> skinFiltered; // indices into the shared ClassCache
				static char skinSearch[128] = "";
				static std::string skinLastKey = "\x01";
				static size_t skinLastCacheSize = SIZE_MAX;
				static std::string skinSelected;

				ImGui::SetNextItemWidth(240.f);
				if (ImGui::BeginCombo("##skinclass", skinSelected.empty() ? "Skin class..." : skinSelected.c_str()))
				{
					const auto& classes = ClassCache::Get();

					ImGui::SetNextItemWidth(-1.f);
					ImGui::InputTextWithHint("##skinsearch", "filter: skin name...", skinSearch, sizeof(skinSearch));

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

					ImGui::BeginChild("##skinlist", ImVec2(340, 220));
					ImGuiListClipper clipper;
					clipper.Begin(static_cast<int>(skinFiltered.size()));
					while (clipper.Step())
						for (int r = clipper.DisplayStart; r < clipper.DisplayEnd; r++)
						{
							const std::string& name = classes[skinFiltered[r]].name;
							if (ImGui::Selectable(name.c_str(), name == skinSelected)) skinSelected = name;
						}
					ImGui::EndChild();
					ImGui::EndCombo();
				}
				ImGui::SameLine();
				if (ImGui::SmallButton("Refresh##skin")) ClassCache::Rebuild();

				ImGui::SameLine();
				if (!isInGame || skinSelected.empty()) ImGui::BeginDisabled();
				if (ImGui::Button("Apply skin") && isInGame && !skinSelected.empty() && Globals::PlayerController)
				{
					UObject* cls = Engine::GObjects->FindObject(skinSelected.c_str());
					auto* character = reinterpret_cast<APortalWarsCharacter*>(Globals::PlayerController->Character);
					if (cls && character)
					{
						character->CharacterSkinClass = reinterpret_cast<ACharacterSkin*>(cls);
						character->UpdateSkins();
						Logger::Log("SUCCESS", "Applied skin: " + skinSelected);
					}
					else
						Logger::Log("ERROR", "Apply skin: class not found: " + skinSelected);
				}
				if (!isInGame || skinSelected.empty()) ImGui::EndDisabled();
				ImGui::Tooltip("Pick a skin class, then apply it to your character, gun or jetpack.\nClient-side (sets the *SkinClass + UpdateSkins); the server may re-assert your real skins. Refresh rescans classes.");

				// Apply the selected class to the gun / jetpack too (they use their own skin types).
				if (!isInGame || skinSelected.empty()) ImGui::BeginDisabled();
				if (ImGui::Button("Apply gun skin") && isInGame && !skinSelected.empty() && Globals::PlayerController)
				{
					UObject* cls = Engine::GObjects->FindObject(skinSelected.c_str());
					auto* character = reinterpret_cast<APortalWarsCharacter*>(Globals::PlayerController->Character);
					if (cls && character && character->CurrentWeapon)
					{
						character->CurrentWeapon->WeaponSkinClass = reinterpret_cast<ABaseGunSkin*>(cls);
						character->CurrentWeapon->UpdateSkins();
						Logger::Log("SUCCESS", "Applied gun skin: " + skinSelected);
					}
				}
				ImGui::SameLine();
				if (ImGui::Button("Apply jetpack skin") && isInGame && !skinSelected.empty() && Globals::PlayerController)
				{
					UObject* cls = Engine::GObjects->FindObject(skinSelected.c_str());
					auto* character = reinterpret_cast<APortalWarsCharacter*>(Globals::PlayerController->Character);
					if (cls && character)
					{
						character->JetpackSkinClass = reinterpret_cast<AJetpackSkin*>(cls);
						character->UpdateSkins();
						Logger::Log("SUCCESS", "Applied jetpack skin: " + skinSelected);
					}
				}
				if (!isInGame || skinSelected.empty()) ImGui::EndDisabled();
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