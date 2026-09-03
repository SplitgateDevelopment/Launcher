#pragma once

#include <functional>
#include <unordered_map>
#include <vector>

// Lightweight, Python-free event registry. Features and user scripts subscribe
// callbacks to events, and the engine dispatches them from the appropriate place
// — Render once per frame, game events (Shutdown, ...) from ProcessEvent.
//
// This header has no pybind11 dependency so it can be included from the hot
// ProcessEvent path and unit tested on its own. modules/Events.h bridges Python
// callbacks (and exposes Type to scripts) onto Register().
namespace Events
{
	// Every dispatchable event. Add a value here to make a new event available to
	// features (Feature::Event) and scripts; for a game event, also add a row to
	// ProcessEvent's table mapping it to a UFunction name.
	enum class Type
	{
		Render,       // every rendered frame
		Shutdown,     // game instance is shutting down
		LoadIntoMap,  // "Load into map" button pressed
		// PlayerDeath, PlayerSpawn, ... (wire in ProcessEvent)
	};

	inline std::unordered_map<Type, std::vector<std::function<void()>>> handlers;

	inline void Register(Type event, std::function<void()> handler)
	{
		handlers[event].push_back(std::move(handler));
	}

	inline bool HasHandlers(Type event)
	{
		auto it = handlers.find(event);
		return it != handlers.end() && !it->second.empty();
	}

	// True when nothing is subscribed at all — a cheap guard for hot paths.
	inline bool Empty()
	{
		return handlers.empty();
	}

	// Invoke every handler subscribed to the event. A throwing handler is
	// contained so it can never unwind into a game hook.
	inline void Dispatch(Type event)
	{
		auto it = handlers.find(event);
		if (it == handlers.end()) return;

		for (auto& handler : it->second)
		{
			try { handler(); }
			catch (...) {}
		}
	}

	inline void Clear()
	{
		handlers.clear();
	}
};
