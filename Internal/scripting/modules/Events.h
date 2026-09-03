#pragma once

#include <pybind11/embed.h>
#include "../Events.h"
#include "../../utils/Logger.h"

/**
 * @file
 * @brief pybind11 module bridging the C++ event bus (Events.h) to user scripts, exposing the
 * `Events::Type` enum and an `on(event, callback)` subscription under `SplitgateInternal.Events`.
 */

namespace py = pybind11;

namespace Scripts
{
	namespace Modules
	{
		/**
		 * Registers the `Events` submodule on @p m: the `Type` enum values plus an `on`
		 * function that subscribes a Python callable to an event. The callable is wrapped so a
		 * failing handler is logged (and the GIL is held while it runs) instead of escaping
		 * into the engine.
		 *
		 * @param m the parent embedded module (SplitgateInternal).
		 *
		 * Usage from a user script:
		 * @code{.py}
		 *   import SplitgateInternal
		 *   def on_shutdown():
		 *       SplitgateInternal.Logger.Log("INFO", "bye")
		 *   SplitgateInternal.Events.on(SplitgateInternal.Events.Shutdown, on_shutdown)
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
				.export_values();

			events.def("on", [](::Events::Type event, py::function callback)
					   { ::Events::Register(event, [callback]()
											{
					try {
						py::gil_scoped_acquire gil;
						callback();
					}
					catch (py::error_already_set& e) {
						Logger::Log("ERROR", e.what());
					} }); });
		}
	} // namespace Modules
} // namespace Scripts
