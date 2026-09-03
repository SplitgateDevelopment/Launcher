#pragma once

/// @file
/// @brief Hooked UE UObject::ProcessEvent — the DLL's central UFunction funnel.
///
/// ProcessEvent is called by the engine for every UFunction invocation, so
/// hooking it lets the DLL observe (and optionally intercept) all game events.
/// This file installs the hook, drives the scripting event bus by mapping
/// selected UFunctions to Events::Type entries (see @ref gameEvents), reacts to
/// game shutdown, and provides an optional LogProcessEvent debug path that
/// prints every non-filtered call.
#include "../../ue/Engine.h"
#include "../../settings/Settings.h"
#include "../../utils/ExceptionHandler.h"
#include "../../scripting/Events.h"

#include <unordered_map>

/// @brief Hook and support code for UObject::ProcessEvent.
namespace ProcessEvent
{
	/// @brief Table mapping user-facing script events to their UFunction names.
	/// Game events surfaced to user scripts (see scripting docs / Events.h).
	// Map an Events::Type to the UFunction full name exactly as printed by
	// LogProcessEvent. Add a value to Events::Type and a row here for each event
	// you want to expose; scripts then subscribe with:
	//   SplitgateInternal.Events.on(SplitgateInternal.Events.PlayerDeath, handler)
	// Candidate UFunction names sourced from the Dumpspace dump (see docs/game-dump.md).
	// A wrong/renamed name simply fails FindObject and no-ops, so confirm the exact string
	// with a LogProcessEvent session in-game (it prints Function->GetFullName()) and correct
	// any that don't fire. payload.source is the calling UObject (the character / controller).
	static const std::pair<Events::Type, const char*> gameEvents[] = {
		{Events::Type::Shutdown, "Function Engine.GameInstance.ReceiveShutdown"},
		{Events::Type::PlayerDeath, "Function PortalWars.PortalWarsCharacter.OnDeath"},
		{Events::Type::HealthChanged, "Function PortalWars.PortalWarsCharacter.OnRep_Health"},
		{Events::Type::DamageTaken, "Function PortalWars.PortalWarsPlayerController.ClientNotifyDamageTaken"},
		{Events::Type::RoundEnded, "Function PortalWars.PortalWarsPlayerController.ClientSetRoundResult"},
		{Events::Type::MatchEnded, "Function PortalWars.PortalWarsPlayerController.ClientSetMatchResult"},
		// Events::Type::PlayerKilled is NOT in this table: it decodes params (killer/victim/
		// headshot) into the payload rather than using the generic {Class}, so it is handled by
		// a dedicated block in HookedProcessEvent below.
	};

	void** VTable;											 ///< VTable the hook is installed into.
	void (*Original)(UObject*, UFunction*, void*) = nullptr; ///< Trampoline to the original ProcessEvent.
	int Index = 68;											 ///< VTable index of ProcessEvent to swap.

	/// @brief Substrings of UFunction names suppressed by LogProcessEvent.
	/// Filters out high-frequency / noisy calls (UI, animation, math helpers)
	/// so the debug log stays readable.
	static std::vector<std::string> filteredWords{
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

	/// @brief Log a single ProcessEvent call, skipping filtered/noisy functions.
	/// @param Class The caller object whose method is being invoked.
	/// @param Function The UFunction being called.
	// Caller gates this on Settings.DEBUG.LogProcessEvent so the hot path pays
	// nothing when logging is off.
	void LogProcessEvent(UObject* Class, UFunction* Function)
	{
		std::string className = Class->GetName().c_str();
		std::string functionName = Function->GetFullName();

		if (className == "DebugCanvasObject") return;
		for (const auto& word : filteredWords)
		{
			if (functionName.find(word) != std::string::npos) return;
		}

		Logger::Log("INFO", std::format("Caller [{}] Function [{}]", className, functionName));
	}

	/// @brief Hooked ProcessEvent: logs, handles shutdown, dispatches script
	/// events, then forwards to the original.
	/// @param Class The object the UFunction is being called on.
	/// @param Function The UFunction being invoked.
	/// @param Params Packed parameter block for the call.
	void HookedProcessEvent(UObject* Class, UFunction* Function, void* Params)
	{
		if (Settings.DEBUG.LogProcessEvent) LogProcessEvent(Class, Function);

		static UObject* ReceiveShutdown = ObjObjects->FindObject("Function Engine.GameInstance.ReceiveShutdown");

		if (Function == ReceiveShutdown)
		{
			Logger::Log("INFO", "Received shutdown");
			ExceptionHandler::Disable();
			Logger::DestroyConsole();
			// Hook::UnHook();
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

		// Rich kill event: decode BroadcastDeath_Multicast's params (killer / victim /
		// headshot) into the payload so handlers get context, not just the caller. Gated on
		// HasHandlers so the params struct is only touched when something is subscribed.
		// Parameter layout comes from the Dumpspace dump (see docs/game-dump.md).
		if (Events::HasHandlers(Events::Type::PlayerKilled))
		{
			static UObject* BroadcastDeath = ObjObjects->FindObject("Function PortalWars.PortalWarsPlayerState.BroadcastDeath_Multicast");

			if (Function == BroadcastDeath)
			{
				// Parameters in declaration order; only the leading fields are read, so the
				// trailing float/TArrays are omitted. Object pointers are 8 bytes each, so the
				// bool lands at offset 0x18.
				struct BroadcastDeathParams
				{
					void* KillerPlayerState; // 0x00
					void* KillerDamageType;	 // 0x08
					void* KilledPlayerState; // 0x10
					bool bIsHeadshot;		 // 0x18
				};

				const auto* params = reinterpret_cast<BroadcastDeathParams*>(Params);
				Events::Dispatch(Events::Type::PlayerKilled,
								 {params->KillerPlayerState, params->KilledPlayerState, params->bIsHeadshot ? 1.f : 0.f});
			}
		}

		// Dispatch registered game events to user scripts. Resolve the
		// name->UFunction table once, then a single map lookup per call;
		// skipped entirely when scripting is off or nothing is subscribed.
		if (Settings.MISC.UserScriptsEnabled && !Events::Empty())
		{
			static const std::unordered_map<UObject*, Events::Type> gameEventByFn = []
			{
				std::unordered_map<UObject*, Events::Type> map;
				for (const auto& [event, name] : gameEvents)
				{
					if (UObject* obj = ObjObjects->FindObject(name)) map[obj] = event;
				}
				return map;
			}();

			auto it = gameEventByFn.find(Function);
			if (it != gameEventByFn.end()) Events::Dispatch(it->second, {Class});
		}

		return Original(Class, Function, Params);
	};
} // namespace ProcessEvent