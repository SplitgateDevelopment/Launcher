#pragma once

#include <algorithm>
#include <functional>
#include <unordered_map>
#include <utility>
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
		PlayerKilled,  ///< a kill happened; payload carries killer/victim/headshot (see ProcessEvent)
		ChatReceived,  ///< an incoming chat message; payload.name holds the message text
	};

	/// Optional data an event can carry. Most events dispatch with a default (empty) payload;
	/// game events can fill in the objects/scalar involved (e.g. source = the pawn that died).
	/// Handlers that don't care ignore it.
	struct Payload
	{
		void* source = nullptr;		///< primary object involved
		void* target = nullptr;		///< secondary object (e.g. the instigator)
		float value = 0.f;			///< a scalar (e.g. damage, or a new bool value 0/1)
		const char* name = nullptr; ///< SettingsChanged: which setting/feature changed (a label)
	};

	/// A registered handler and the id Register handed back (so it can be Unregister-ed).
	using Entry = std::pair<int, std::function<void(const Payload&)>>;

	/// Subscribers, keyed by event; iterated in registration order on Dispatch.
	inline std::unordered_map<Type, std::vector<Entry>> handlers;

	/// Monotonic handler id source (0 is never used, so it can mean "no handle").
	inline int nextHandlerId = 1;

	/**
	 * Subscribe a payload-aware handler to an event.
	 * @param event   the event to subscribe to.
	 * @param handler invoked with the dispatched payload; kept until Unregister / Clear.
	 * @return an id that Unregister can later use to remove this handler.
	 */
	inline int Register(Type event, std::function<void(const Payload&)> handler)
	{
		const int id = nextHandlerId++;
		handlers[event].push_back({id, std::move(handler)});
		return id;
	}

	/**
	 * Convenience overload for handlers that don't need the payload.
	 * @param event   the event to subscribe to.
	 * @param handler invoked with no arguments on dispatch.
	 * @return the handler id (see the payload overload).
	 */
	inline int Register(Type event, std::function<void()> handler)
	{
		const int id = nextHandlerId++;
		handlers[event].push_back({id, [h = std::move(handler)](const Payload&)
								   { h(); }});
		return id;
	}

	/**
	 * Remove a handler previously added by Register, by its returned id. Scans every event, so an
	 * id need not carry its event. Used to un-subscribe script handlers on hot-reload so they don't
	 * stack. A no-op if the id isn't found.
	 */
	inline void Unregister(int id)
	{
		for (auto& [type, list] : handlers)
			list.erase(std::remove_if(list.begin(), list.end(), [id](const Entry& e)
									  { return e.first == id; }),
					   list.end());
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

		for (auto& [id, handler] : it->second)
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
