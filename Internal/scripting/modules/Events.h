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
		//   def on_death():
		//       SplitgateInternal.Logger.Log("INFO", "died")
		//
		//   SplitgateInternal.Events.on("player_death", on_death)
		//
		// The Python callable is wrapped so a failing handler is logged (and the
		// GIL is held while it runs) instead of escaping into the engine.
		void Events(py::module_& m) {
			auto events = m.def_submodule("Events");
			events.def("on", [](const std::string& event, py::function callback) {
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
