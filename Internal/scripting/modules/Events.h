#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
#include <pybind11/embed.h>
#include "../Events.h"
#include "../../utils/Logger.h"

/**
 * @file
 * @brief pybind11 module bridging the C++ event bus (Events.h) to user scripts, exposing the
 * `Events::Type` enum, the `Events::Payload`, and an `on(event, callback)` subscription under
 * `SplitgateInternal.Events`.
 */

namespace py = pybind11;

namespace Scripts
{
	namespace Modules
	{
		/// Script-defined string events (separate from the C++ Events bus): name -> (callback, wantsArg).
		inline std::unordered_map<std::string, std::vector<std::pair<py::function, bool>>> customHandlers;

		/// Ids of bus handlers a script registered via `Events.on`, so hot-reload can remove exactly
		/// those (not the C++ feature handlers) before re-importing.
		inline std::vector<int> scriptHandlerIds;

		/// Drop every script-registered custom handler (used by hot-reload so they don't stack).
		inline void ClearCustomEvents() { customHandlers.clear(); }

		/// Un-subscribe every bus handler a script registered via `Events.on` (hot-reload).
		inline void ClearScriptHandlers()
		{
			for (int id : scriptHandlerIds) ::Events::Unregister(id);
			scriptHandlerIds.clear();
		}

		/**
		 * Registers the `Events` submodule on @p m: the `Type` enum values, the `Payload`
		 * type, and an `on` function that subscribes a Python callable to an event. The
		 * callable is wrapped so a failing handler is logged (and the GIL is held while it
		 * runs) instead of escaping into the engine.
		 *
		 * A handler may take zero arguments, or one argument to receive the event's `Payload`
		 * (arity is detected once at registration, so existing no-arg handlers keep working).
		 *
		 * @param m the parent embedded module (SplitgateInternal).
		 *
		 * Usage from a user script:
		 * @code{.py}
		 *   import SplitgateInternal as SG
		 *   def on_kill(payload):
		 *       # payload.source / payload.target are raw object addresses (ints);
		 *       # payload.value is event-specific (headshot flag for PlayerKilled).
		 *       SG.Logger.Log("INFO", "headshot!" if payload.value else "kill")
		 *   SG.Events.on(SG.Events.PlayerKilled, on_kill)
		 * @endcode
		 */
		void Events(py::module_& m)
		{
			auto events = m.def_submodule("Events");

			py::enum_<::Events::Type>(events, "Type")
				.value("Render", ::Events::Type::Render)
				.value("Shutdown", ::Events::Type::Shutdown)
				.value("LoadIntoMap", ::Events::Type::LoadIntoMap)
				.value("SettingsChanged", ::Events::Type::SettingsChanged)
				.value("MenuOpened", ::Events::Type::MenuOpened)
				.value("MenuClosed", ::Events::Type::MenuClosed)
				.value("PlayerDeath", ::Events::Type::PlayerDeath)
				.value("HealthChanged", ::Events::Type::HealthChanged)
				.value("DamageTaken", ::Events::Type::DamageTaken)
				.value("RoundEnded", ::Events::Type::RoundEnded)
				.value("MatchEnded", ::Events::Type::MatchEnded)
				.value("PlayerKilled", ::Events::Type::PlayerKilled)
				.value("ChatReceived", ::Events::Type::ChatReceived)
				.export_values();

			// Payload exposed to scripts. source/target are raw object addresses (game object
			// pointers as ints — advanced use); value is an event-specific scalar.
			py::class_<::Events::Payload>(events, "Payload")
				.def_property_readonly("source", [](const ::Events::Payload& p)
									   { return reinterpret_cast<uintptr_t>(p.source); })
				.def_property_readonly("target", [](const ::Events::Payload& p)
									   { return reinterpret_cast<uintptr_t>(p.target); })
				.def_readonly("value", &::Events::Payload::value)
					.def_property_readonly("name", [](const ::Events::Payload& p)
										   { return std::string(p.name ? p.name : ""); }); // e.g. ChatReceived message text

			events.def("on", [](::Events::Type event, py::function callback)
					   {
				// Detect once whether the handler wants the payload argument, so both
				// `def f():` and `def f(payload):` work.
				bool wantsPayload = false;
				try
				{
					py::gil_scoped_acquire gil;
					auto params = py::module_::import("inspect").attr("signature")(callback).attr("parameters");
					wantsPayload = py::len(params) >= 1;
				}
				catch (py::error_already_set&)
				{
					// e.g. a builtin with no introspectable signature — assume no payload.
				}

				const int id = ::Events::Register(event, [callback, wantsPayload](const ::Events::Payload& payload)
								   {
					try
					{
						py::gil_scoped_acquire gil;
						if (wantsPayload) callback(payload);
						else callback();
					}
					catch (py::error_already_set& e)
					{
						Logger::Log("ERROR", e.what());
					} });
				scriptHandlerIds.push_back(id); });

			// Script-to-script custom events: subscribe by name, and emit(name, value=None) from
			// another script. Handlers may take the value or not (arity detected once).
			events.def("on_custom", [](std::string name, py::function callback)
					   {
				bool wantsArg = false;
				try
				{
					py::gil_scoped_acquire gil;
					auto params = py::module_::import("inspect").attr("signature")(callback).attr("parameters");
					wantsArg = py::len(params) >= 1;
				}
				catch (py::error_already_set&) {}
				customHandlers[name].push_back({callback, wantsArg}); }, py::arg("name"), py::arg("callback"));

			events.def("emit", [](std::string name, py::object value)
					   {
				auto it = customHandlers.find(name);
				if (it == customHandlers.end()) return;
				for (auto& [callback, wantsArg] : it->second)
				{
					try
					{
						py::gil_scoped_acquire gil;
						if (wantsArg) callback(value);
						else callback();
					}
					catch (py::error_already_set& e)
					{
						Logger::Log("ERROR", e.what());
					}
				} }, py::arg("name"), py::arg("value") = py::none());
		}
	} // namespace Modules
} // namespace Scripts
