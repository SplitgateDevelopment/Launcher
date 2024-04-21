#pragma once

#include "../../settings/Settings.h"
#include "../../hook/Hook.h"

namespace Menu {
	namespace Sections {
		void DebugTab() {
			ImGui::ToggleButton("Log ProcessEvent", &Settings.DEBUG.LogProcessEvent);
			ImGui::ToggleButton("Show demo window", &Settings.DEBUG.ShowDemoWindow);
			ImGui::ToggleButton("Show style editor", &Settings.DEBUG.ShowStyleEditor);
			ImGui::ToggleButton("Draw Actors", &Settings.DEBUG.DrawActors);
			
			if (ImGui::Button("Summon Bot"))
			{
				if (!Globals::PlayerController) return;
				if (!Globals::PlayerController->IsInGame()) return;

				Globals::PlayerController->SendToConsole(FString("summon PortalWarsBot_BP_C"));
				Globals::PlayerController->SendChatMessage(FString("Summoned actor"));
			}

			if (ImGui::Button("Dump GObjects"))
			{
				fs::path dumpsDir = SettingsHelper::GetAppPath() / "Dumps";
				if (!fs::exists(dumpsDir)) fs::create_directories(dumpsDir);

				fs::path filePath = dumpsDir / "GObjects.txt";

				std::ofstream file(filePath, std::ios::out | std::ios::trunc);
				if (!file.is_open()) {
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
				Globals::PlayerController->SendChatMessage(FString(msg));
			}
		}
	}
}