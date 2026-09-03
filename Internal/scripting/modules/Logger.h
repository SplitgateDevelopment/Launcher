#pragma once

#include <pybind11/embed.h>
#include "../../utils/Logger.h"

/**
 * @file
 * @brief pybind11 module exposing the in-game logger to user scripts as
 * `SplitgateInternal.Logger.Log(level, message)`.
 */

namespace py = pybind11;

namespace Scripts
{
	namespace Modules
	{
		/// Registers the `Logger` submodule (a single `Log(level, message)` binding) on @p m.
		/// @param m the parent embedded module (SplitgateInternal).
		void Logger(py::module_& m)
		{
			auto logger = m.def_submodule("Logger");
			logger.def("Log", &Logger::Log);
		}
	} // namespace Modules
} // namespace Scripts