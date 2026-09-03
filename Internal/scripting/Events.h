#pragma once

#include <string>
#include <functional>
#include <unordered_map>
#include <vector>

// Lightweight, Python-free event registry. User scripts subscribe callbacks to
// named events (via the SplitgateInternal.Events module, see modules/Events.h),
// and the engine dispatches those events from the appropriate place — "render"
// once per frame from the UserScripts feature, game events (e.g. "shutdown")
// from ProcessEvent.
//
// This header deliberately has no pybind11 dependency so it can be included from
// the hot ProcessEvent path and unit tested on its own. modules/Events.h bridges
// Python callbacks onto Register().
namespace Events
{
	inline std::unordered_map<std::string, std::vector<std::function<void()>>> handlers;

	inline void Register(const std::string& event, std::function<void()> handler)
	{
		handlers[event].push_back(std::move(handler));
	}

	inline bool HasHandlers(const std::string& event)
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
	inline void Dispatch(const std::string& event)
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
