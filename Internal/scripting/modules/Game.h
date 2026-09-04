#pragma once

#include <string>

#include <pybind11/embed.h>
#include <imgui.h>

#include "../../ue/Engine.h"
#include "../../utils/Globals.h"

/**
 * @file
 * @brief pybind11 module exposing lightweight game-state reads to user scripts as
 * `SplitgateInternal.Game.*` — frame rate, current map/world name, and the local player's name.
 */

namespace py = pybind11;

namespace Scripts
{
	namespace Modules
	{
		/// Registers the `Game` submodule.
		void Game(py::module_& m)
		{
			auto game = m.def_submodule("Game");

			game.def("fps", []
					 { return ImGui::GetIO().Framerate; }, "Current frame rate (ImGui's smoothed estimate).");

			game.def("map_name", []() -> py::object
					 {
				if (!Globals::World) return py::none();
				return py::str(Globals::World->GetName()); }, "The current world/map object name, or None.");

			game.def("local_name", []() -> py::object
					 {
				if (!Globals::PlayerController) return py::none();
				auto* c = reinterpret_cast<APortalWarsCharacter*>(Globals::PlayerController->Character);
				if (!c || !c->PlayerState) return py::none();
				return py::str(c->PlayerState->PlayerNamePrivate.ToString()); }, "The local player's display name, or None.");
		}
	} // namespace Modules
} // namespace Scripts
