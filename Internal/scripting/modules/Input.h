#pragma once

#include <Windows.h>

#include <pybind11/embed.h>

/**
 * @file
 * @brief pybind11 module exposing keyboard/mouse polling to user scripts as
 * `SplitgateInternal.Input.*`, so a script can gate behavior on a key (uses virtual-key codes,
 * e.g. 0x01 = left mouse, 0x74 = F5).
 */

namespace py = pybind11;

namespace Scripts
{
	namespace Modules
	{
		/// Registers the `Input` submodule (held / just-pressed polling by virtual-key code).
		void Input(py::module_& m)
		{
			auto input = m.def_submodule("Input");

			input.def("is_key_down", [](int vk)
					  { return (GetAsyncKeyState(vk) & 0x8000) != 0; }, py::arg("vk"),
					  "True while the virtual-key is held down.");

			input.def("is_key_pressed", [](int vk)
					  { return (GetAsyncKeyState(vk) & 1) != 0; }, py::arg("vk"),
					  "True once per press (the low bit is set since the last poll).");
		}
	} // namespace Modules
} // namespace Scripts
