#pragma once

#include <pybind11/embed.h>
#include "../../utils/Logger.h"

namespace py = pybind11;

namespace Scripts {
	namespace Modules {
		void Logger(py::module_& m) {
			auto logger = m.def_submodule("Logger");
			logger.def("Log", &Logger::Log);
		}
	}
}