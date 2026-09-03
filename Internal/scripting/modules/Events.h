#pragma once

#include <pybind11/embed.h>
#include "../Events.h"
#include "../../utils/Logger.h"

namespace py = pybind11;

namespace Scripts {
	namespace Modules {
		// Exposes SplitgateInternal.Events to user scripts:
		//
		//   import SplitgateInternal
		//
		//   def on_shutdown():
		//       SplitgateInternal.Logger.Log("INFO", "bye")
		//
		//   SplitgateInternal.Events.on(SplitgateInternal.Events.Shutdown, on_shutdown)
		//
		// The Python callable is wrapped so a failing handler is logged (and the
		// GIL is held while it runs) instead of escaping into the engine.
		void Events(py::module_& m) {
			auto events = m.def_submodule("Events");

			py::enum_<::Events::Type>(events, "Type")
				.value("Render", ::Events::Type::Render)
				.value("Shutdown", ::Events::Type::Shutdown)
				.value("LoadIntoMap", ::Events::Type::LoadIntoMap)
				.export_values();

			events.def("on", [](::Events::Type event, py::function callback) {
				::Events::Register(event, [callback]() {
					try {
						py::gil_scoped_acquire gil;
						callback();
					}
					catch (py::error_already_set& e) {
						Logger::Log("ERROR", e.what());
					}
				});
			});
		}
	}
}
