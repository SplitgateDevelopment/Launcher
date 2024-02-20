#pragma once

#include <pybind11/embed.h>
#include "../../settings/Settings.h"

namespace py = pybind11;

namespace Scripts {
	namespace Modules {
		void Settings(py::module_& m) {
			auto settings = m.def_submodule("Settings");
			settings.def("Reset", &SettingsHelper::Reset);
			settings.def("Save", &SettingsHelper::Save);
		}
	}
}