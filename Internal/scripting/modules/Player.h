#pragma once

#include <string>

#include <pybind11/embed.h>

#include "../../ue/Engine.h"
#include "../../utils/Globals.h"

/**
 * @file
 * @brief pybind11 module exposing the local character/controller to user scripts as
 * `SplitgateInternal.Player.*` — location/teleport, health, view rotation, console, and chat.
 * Every call guards on a valid, in-game controller and pawn, so a script can't crash the game by
 * poking a null pointer.
 */

namespace py = pybind11;

namespace Scripts
{
	namespace Modules
	{
		/// The local pawn (AcknowledgedPawn) or nullptr if not in game.
		inline AActor* LocalPawn()
		{
			if (!Globals::PlayerController) return nullptr;
			return reinterpret_cast<AActor*>(Globals::PlayerController->AcknowledgedPawn);
		}

		/// The local character (for health / character-specific fields) or nullptr.
		inline APortalWarsCharacter* LocalCharacter()
		{
			if (!Globals::PlayerController) return nullptr;
			return reinterpret_cast<APortalWarsCharacter*>(Globals::PlayerController->Character);
		}

		/// Registers the `Player` submodule: local location/teleport, health, view rotation, and the
		/// console/chat helpers.
		void Player(py::module_& m)
		{
			auto player = m.def_submodule("Player");

			player.def("is_in_game", []
					   { return Globals::PlayerController && Globals::PlayerController->IsInGame(); });

			player.def("location", []() -> py::object
					   {
				auto* pawn = LocalPawn();
				if (!pawn) return py::none();
				FVector v = pawn->K2_GetActorLocation();
				return py::make_tuple(v.X, v.Y, v.Z); });

			// Teleport keeping the current view rotation. Returns whether the move succeeded.
			player.def("teleport", [](float x, float y, float z) -> bool
					   {
				auto* pawn = LocalPawn();
				if (!pawn) return false;
				FRotator rot = Globals::PlayerController->ControlRotation;
				return pawn->K2_TeleportTo(FVector{x, y, z}, rot); }, py::arg("x"), py::arg("y"), py::arg("z"));

			player.def("health", []() -> py::object
					   {
				auto* c = LocalCharacter();
				if (!c) return py::none();
				return py::float_(c->Health); });

			player.def("set_health", [](float value)
					   {
				auto* c = LocalCharacter();
				if (c) c->Health = value; }, py::arg("value"));

			player.def("view_rotation", []() -> py::object
					   {
				if (!Globals::PlayerController) return py::none();
				FRotator r = Globals::PlayerController->ControlRotation;
				return py::make_tuple(r.Pitch, r.Yaw, r.Roll); });

			player.def("set_view_rotation", [](float pitch, float yaw, float roll)
					   {
				if (!Globals::PlayerController) return;
				FRotator r{pitch, yaw, roll};
				Globals::PlayerController->SetControlRotation(r); }, py::arg("pitch"), py::arg("yaw"), py::arg("roll") = 0.f);

			player.def("console", [](std::string command)
					   {
				if (Globals::PlayerController) Globals::PlayerController->SendToConsole(FString(command)); }, py::arg("command"));

			player.def("chat", [](std::string message)
					   {
				if (Globals::PlayerController) Globals::PlayerController->SendChatMessage(FString(message)); }, py::arg("message"));
		}
	} // namespace Modules
} // namespace Scripts
