#pragma once

#include "../../ue/Engine.h"
#include "../../settings/Settings.h"
#include "../../utils/ExceptionHandler.h"
#include "../../scripting/Events.h"

#include <unordered_map>

namespace ProcessEvent {
	// Game events surfaced to user scripts (see scripting docs / Events.h).
	// Map an event name to the UFunction full name exactly as printed by
	// LogProcessEvent, then scripts can subscribe with:
	//   SplitgateInternal.Events.on("player_death", handler)
	// Add a row here for each event you want to expose.
	static const std::pair<const char*, const char*> gameEvents[] = {
		{ "shutdown", "Function Engine.GameInstance.ReceiveShutdown" },
		// { "player_death", "Function PortalWars.PortalWarsCharacter.OnDeath" },
	};

	void** VTable;
	void (*Original)(UObject*, UFunction*, void*) = nullptr;
	int Index = 68;

	static std::vector<std::string> filteredWords
	{
		"SetName",
		"ServerChangeName",
		"FOV",
		"UMG",
		"CameraModifier",
		"BlueprintUpdateAnimation",
		"BlueprintPostEvaluateAnimation",
		"KismetMathLibrary",
		"KismetTextLibrary",
		"Construct",
		"PopulateItems",
		"WBP",
		"IsInputActionEnabled",
		"GetNewVisibility",
		"Focus",
		"OnControllerStatusChanged",
		"GetDataContext",
		"Hover",
		"Unhover",
		"OnText",
		"OnInitiallyDisplayed",
		"OnDataContextChanged",
		"DelegateSignature",
		"GetActorEnableCollision",
		"PerformConditionCheck",
		"LocalizedMessage",
		"ReadyToEndMatch",
		"ClientHearAk",
		"OnActorBump",
		"Received_Notify",
	};

	// Caller gates this on Settings.DEBUG.LogProcessEvent so the hot path pays
	// nothing when logging is off.
	void LogProcessEvent(UObject* Class, UFunction* Function)
	{
		std::string className = Class->GetName().c_str();
		std::string functionName = Function->GetFullName();

		if (className == "DebugCanvasObject") return;
		for (const auto& word : filteredWords) {
			if (functionName.find(word) != std::string::npos) return;
		}

		Logger::Log("INFO", std::format("Caller [{}] Function [{}]", className, functionName));
	}

	void HookedProcessEvent(UObject* Class, UFunction* Function, void* Params) {
		if (Settings.DEBUG.LogProcessEvent) LogProcessEvent(Class, Function);

		static UObject* ReceiveShutdown = ObjObjects->FindObject("Function Engine.GameInstance.ReceiveShutdown");

		if (Function == ReceiveShutdown) {
			Logger::Log("INFO", "Received shutdown");
			ExceptionHandler::Disable();
			Logger::DestroyConsole();
			//Hook::UnHook();
		};

		/* 
		TODO: Intercept and modify widget IsInputActionEnabled function
		static UObject* IsInputActionEnabled = ObjObjects->FindObject("Function PortalWars.PortalWarsUserWidget.IsInputActionEnabled");
		//Pointer comparison is faster
		if (Function == IsInputActionEnabled) {
			Logger::Log("INFO", "IsInputActionEnabled intercepted");

			struct IsInputActionEnabledStruct {
				struct FGameplayTag& ActionTag;
				bool& InIsEnabled;
			} *IsInputActionEnabledParams;
			IsInputActionEnabledParams = reinterpret_cast<IsInputActionEnabledStruct*>(Params);

			std::cout << IsInputActionEnabledParams << std::endl;
			
			IsInputActionEnabledParams->InIsEnabled = true;

			Logger::Log("INFO", "IsInputActionEnabled modified");
			return;
		}*/

		// Dispatch registered game events to user scripts. Resolve the
			// name->UFunction table once, then a single map lookup per call;
			// skipped entirely when scripting is off or nothing is subscribed.
			if (Settings.MISC.UserScriptsEnabled && !Events::Empty()) {
				static const std::unordered_map<UObject*, const char*> gameEventByFn = [] {
					std::unordered_map<UObject*, const char*> map;
					for (const auto& [event, name] : gameEvents) {
						if (UObject* obj = ObjObjects->FindObject(name)) map[obj] = event;
					}
					return map;
				}();

				auto it = gameEventByFn.find(Function);
				if (it != gameEventByFn.end()) Events::Dispatch(it->second);
			}

			return Original(Class, Function, Params);
	};
}