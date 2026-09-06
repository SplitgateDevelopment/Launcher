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

		// Diagnostic: log the camera-mode FName the game sets (ClientSetCameraMode's param), so a
		// custom third person can match the game's own mode name. Only while LogProcessEvent is on;
		// trigger a killcam/spectate (which shows third person natively) and read internal.log.
		if (Settings.DEBUG.LogProcessEvent && Params)
		{
			static UObject* clientSetCameraMode = Engine::ObjObjects->FindObject("Function Engine.PlayerController.ClientSetCameraMode");
			if (clientSetCameraMode && Function == clientSetCameraMode)
				Logger::Log("INFO", "[Camera] ClientSetCameraMode -> " + reinterpret_cast<FName*>(Params)->GetName());
		}

		// Shutdown is no longer special-cased here: it's a normal game event
		// (Events::Type::Shutdown in the gameEvents table), and the teardown handler is
		// registered on the event bus in Hook::Init.

		// Force-enable UI input actions (e.g. a greyed-out Play button). The widget calls
		// IsInputActionEnabled(FGameplayTag ActionTag, bool& InIsEnabled) as an out-param
		// (returns void). We let the game compute the value, then override the out-param to
		// true. Gated on a setting because it enables *every* input action, not just Play.
		if (Settings.EXPLOITS.EnableAllInput)
		{
			static UObject* IsInputActionEnabled = Engine::ObjObjects->FindObject("Function PortalWars.PortalWarsUserWidget.IsInputActionEnabled");

			if (Function == IsInputActionEnabled)
			{
				// Params in declaration order: FGameplayTag (8 bytes) then the bool out-param.
				struct IsInputActionEnabledParams
				{
					FGameplayTag ActionTag;
					bool InIsEnabled;
				};

				Original(Class, Function, Params); // compute the real value first
				reinterpret_cast<IsInputActionEnabledParams*>(Params)->InIsEnabled = true;
				return;
			}
		}

		// Rich kill event: decode BroadcastDeath_Multicast's params (killer / victim /
		// headshot) into the payload so handlers get context, not just the caller. Gated on
		// HasHandlers so the params struct is only touched when something is subscribed.
		// Parameter layout comes from the Dumpspace dump (see docs/game-dump.md).
		if (Events::HasHandlers(Events::Type::PlayerKilled))
		{
			static UObject* BroadcastDeath = Engine::ObjObjects->FindObject("Function PortalWars.PortalWarsPlayerState.BroadcastDeath_Multicast");

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

		// Incoming chat: decode ClientUpdateChat's FTextChatData and surface the message text to
		// scripts (payload.name). Gated on HasHandlers so the params are only read when subscribed.
		if (Events::HasHandlers(Events::Type::ChatReceived))
		{
			static UObject* ClientUpdateChat = Engine::ObjObjects->FindObject("Function PortalWars.PortalWarsPlayerController.ClientUpdateChat");

			if (Function == ClientUpdateChat)
			{
				auto* data = reinterpret_cast<FTextChatData*>(Params);
				std::string text = data->NiceText.ToString(); // usually "Name: message"
				if (text.empty()) text = data->SenderText.ToString();
				Events::Dispatch(Events::Type::ChatReceived, {Class, nullptr, 0.f, text.c_str()});
			}
		}

		// Dispatch registered game events to their subscribers (features, the shutdown
		// teardown, and user scripts). Resolve the name->UFunction table once, then a single
		// map lookup per call; skipped entirely when nothing is subscribed. (No longer gated on
		// UserScriptsEnabled, so C++ subscribers like the shutdown handler always fire.)
		if (!Events::Empty())
		{
			static const std::unordered_map<UObject*, Events::Type> gameEventByFn = []
			{
				std::unordered_map<UObject*, Events::Type> map;
				for (const auto& [event, name] : gameEvents)
				{
					if (UObject* obj = Engine::ObjObjects->FindObject(name)) map[obj] = event;
				}
				return map;
			}();

			auto it = gameEventByFn.find(Function);
			if (it != gameEventByFn.end()) Events::Dispatch(it->second, {Class});
		}

		return Original(Class, Function, Params);
	};
} // namespace ProcessEvent