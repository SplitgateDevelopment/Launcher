#pragma once

#include <functional>
#include <unordered_map>
#include <vector>

/**
 * @file
 * @brief Lightweight, Python-free event registry.
 *
 * Features and user scripts subscribe callbacks to events, and the engine dispatches them
 * from the appropriate place — Render once per frame, game events (Shutdown, ...) from
 * ProcessEvent.
 *
 * This header has no pybind11 dependency so it can be included from the hot ProcessEvent
 * path and unit tested on its own. modules/Events.h bridges Python callbacks (and exposes
 * Type to scripts) onto Register().
 */
namespace Events
{
	/// Every dispatchable event. Add a value here to make a new event available to features
	/// (Feature::Event) and scripts; for a game event, also add a row to ProcessEvent's table
	/// mapping it to a UFunction name.
	enum class Type
	{
		Render,			 ///< every rendered frame
		Shutdown,		 ///< game instance is shutting down
		LoadIntoMap,	 ///< "Load into map" button pressed
		SettingsChanged, ///< a setting was changed in the menu
		MenuOpened,		 ///< the GUI was shown
		MenuClosed,		 ///< the GUI was hidden

		// Game events dispatched from ProcessEvent (payload.source = the calling UObject).
		// UFunction names are wired in ProcessEvent's gameEvents table.
		PlayerDeath,   ///< a character died (PortalWarsCharacter.OnDeath)
		HealthChanged, ///< a character's health replicated (PortalWarsCharacter.OnRep_Health)
		DamageTaken,   ///< the local player took damage (PortalWarsPlayerController.ClientNotifyDamageTaken)
		RoundEnded,	   ///< a round ended (PortalWarsPlayerController.ClientSetRoundResult)
		MatchEnded,	   ///< the match ended (PortalWarsPlayerController.ClientSetMatchResult)
	};

	/// Optional data an event can carry. Most events dispatch with a default (empty) payload;
	/// game events can fill in the objects/scalar involved (e.g. source = the pawn that died).
	/// Handlers that don't care ignore it.
	struct Payload
	{
		void* source = nullptr; ///< primary object involved
		void* target = nullptr; ///< secondary object (e.g. the instigator)
		float value = 0.f;		///< a scalar (e.g. damage)
	};

	/// Subscribers, keyed by event; iterated in registration order on Dispatch.
	inline std::unordered_map<Type, std::vector<std::function<void(const Payload&)>>> handlers;

	/**
	 * Subscribe a payload-aware handler to an event.
	 * @param event   the event to subscribe to.
	 * @param handler invoked with the dispatched payload; kept for the registry's lifetime.
	 */
	inline void Register(Type event, std::function<void(const Payload&)> handler)
	{
		handlers[event].push_back(std::move(handler));
	}

	/**
	 * Convenience overload for handlers that don't need the payload.
	 * @param event   the event to subscribe to.
	 * @param handler invoked with no arguments on dispatch.
	 */
	inline void Register(Type event, std::function<void()> handler)
	{
		handlers[event].push_back([h = std::move(handler)](const Payload&)
								  { h(); });
	}

	/// @return true if at least one handler is subscribed to @p event.
	inline bool HasHandlers(Type event)
	{
		auto it = handlers.find(event);
		return it != handlers.end() && !it->second.empty();
	}

	/// @return true when nothing is subscribed at all — a cheap guard for hot paths.
	inline bool Empty()
	{
		return handlers.empty();
	}

	/**
	 * Invoke every handler subscribed to @p event with @p payload. A throwing handler is
	 * contained so it can never unwind into a game hook.
	 * @param event   the event to dispatch.
	 * @param payload optional data; defaults to an empty payload.
	 */
	inline void Dispatch(Type event, const Payload& payload = {})
	{
		auto it = handlers.find(event);
		if (it == handlers.end()) return;

		for (auto& handler : it->second)
		{
			try
			{
				handler(payload);
			}
			catch (...)
			{
			}
		}
	}

	/// Remove all subscribers (used by tests and on teardown).
	inline void Clear()
	{
		handlers.clear();
	}
}; // namespace Events
