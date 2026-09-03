#pragma once

#include <pybind11/embed.h>
#include "../../settings/Settings.h"

/**
 * @file
 * @brief pybind11 module exposing settings control to user scripts as
 * `SplitgateInternal.Settings.Reset()` / `.Save()`.
 */

namespace py = pybind11;

namespace Scripts
{
	namespace Modules
	{
		/// Registers the `Settings` submodule (`Reset` and `Save` bindings) on @p m.
		/// @param m the parent embedded module (SplitgateInternal).
		void Settings(py::module_& m)
		{
			auto settings = m.def_submodule("Settings");
			settings.def("Reset", &SettingsHelper::Reset);
			settings.def("Save", &SettingsHelper::Save);
		}
	} // namespace Modules
} // namespace Scripts