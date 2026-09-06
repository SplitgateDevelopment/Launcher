#pragma once

#include <cmath>
#include <cstdint>
#include <string>

#include <pybind11/embed.h>

#include "../../ue/Engine.h"
#include "../../ue/Globals.h"
#include "../../native/WorldToScreen.h"

/**
 * @file
 * @brief pybind11 module exposing a curated, safe subset of engine utilities to user scripts as
 * `SplitgateInternal.Engine.*` — object lookup by name, native world->screen projection, the
 * viewport size, and a distance helper. Deliberately not the raw SDK.
 */

namespace py = pybind11;

namespace Scripts
{
	namespace Modules
	{
		/// Registers the `Engine` submodule.
		void Engine(py::module_& m)
		{
			auto engine = m.def_submodule("Engine");

			// Resolve a UObject by its full name (e.g. "Class PortalWars.PortalWarsCharacter").
			// Returns the object address as an int (0 if not found) for advanced use.
			engine.def("find_object", [](std::string name) -> std::uintptr_t
					   {
				if (!Engine::GObjects) return 0;
				return reinterpret_cast<std::uintptr_t>(Engine::GObjects->FindObject(name.c_str())); }, py::arg("name"));

			// Project a world point to screen space; returns (x, y) or None if behind the camera /
			// not ready. Honors the Debug native/UFunction projection toggle.
			engine.def("world_to_screen", [](float x, float y, float z) -> py::object
					   {
				FVector2D out{};
				if (Projection::WorldToScreen(FVector{x, y, z}, out)) return py::make_tuple(out.X, out.Y);
				return py::none(); }, py::arg("x"), py::arg("y"), py::arg("z"));

			// Viewport size in pixels, or None if the canvas isn't ready.
			engine.def("canvas_size", []() -> py::object
					   {
				if (!Globals::Canvas) return py::none();
				return py::make_tuple(Globals::Canvas->ClipX, Globals::Canvas->ClipY); });

			// Euclidean distance between two world points, in metres (UE units are cm).
			engine.def("distance", [](float x1, float y1, float z1, float x2, float y2, float z2)
					   {
				const float dx = x1 - x2, dy = y1 - y2, dz = z1 - z2;
				return std::sqrt(dx * dx + dy * dy + dz * dz) / 100.f; },
					   py::arg("x1"), py::arg("y1"), py::arg("z1"), py::arg("x2"), py::arg("y2"), py::arg("z2"));
		}
	} // namespace Modules
} // namespace Scripts
