#pragma once

/// @file
/// @brief Misc tab: player FOV/speed sliders, load-into-map / summon-bot actions, overlay/program
/// controls, and the announce-toggles switch. (User scripts moved to the Scripts tab.)

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

			ImGui::SameLine();
			if (ImGui::Button("Summon Bot") && isInGame && Globals::PlayerController)
			{
				// Prefer a real deferred spawn if the bot class resolves (find its exact name in the
				// SDK tab); otherwise fall back to the console summon.
				UObject* botClass = ObjObjects->FindObject("BlueprintGeneratedClass PortalWarsBot_BP.PortalWarsBot_BP_C");
				auto* pawn = Globals::PlayerController->AcknowledgedPawn;
				if (botClass && pawn)
				{
					FVector loc = reinterpret_cast<AActor*>(pawn)->K2_GetActorLocation();
					loc.X += 200.f; // a bit in front
					AActor* bot = SpawnActor(reinterpret_cast<UObject*>(Globals::PlayerController), reinterpret_cast<UClass*>(botClass),
											 loc, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn, nullptr);
					Logger::Log(bot ? "SUCCESS" : "ERROR", bot ? "Spawned bot" : "SpawnActor failed");
				}
				else
				{
					Globals::PlayerController->SendToConsole(FString("summon PortalWarsBot_BP_C"));
					Logger::Log("INFO", "Summon Bot: class not resolved, used console (find the class in the SDK tab)");
				}
			}
			ImGui::Tooltip("Spawn a bot. Uses a deferred SpawnActor when the bot class resolves; else the\nconsole summon. Find the exact bot class in the SDK tab.");

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