#pragma once

#include <cmath>
#include <string>

#include <pybind11/embed.h>

#include "../../ue/Engine.h"

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
			if (!Engine::PlayerController) return nullptr;
			return reinterpret_cast<AActor*>(Engine::PlayerController->AcknowledgedPawn);
		}

		/// The local character (for health / character-specific fields) or nullptr.
		inline APortalWarsCharacter* LocalCharacter()
		{
			if (!Engine::PlayerController) return nullptr;
			return reinterpret_cast<APortalWarsCharacter*>(Engine::PlayerController->Character);
		}

		/// Registers the `Player` submodule: local location/teleport, health, view rotation, and the
		/// console/chat helpers.
		void Player(py::module_& m)
		{
			auto player = m.def_submodule("Player");

			player.def("is_in_game", []
					   { return Engine::PlayerController && Engine::PlayerController->IsInGame(); });

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
				FRotator rot = Engine::PlayerController->ControlRotation;
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
				if (!Engine::PlayerController) return py::none();
				FRotator r = Engine::PlayerController->ControlRotation;
				return py::make_tuple(r.Pitch, r.Yaw, r.Roll); });

			player.def("set_view_rotation", [](float pitch, float yaw, float roll)
					   {
				if (!Engine::PlayerController) return;
				FRotator r{pitch, yaw, roll};
				Engine::PlayerController->SetControlRotation(r); }, py::arg("pitch"), py::arg("yaw"), py::arg("roll") = 0.f);

			player.def("velocity", []() -> py::object
					   {
				auto* pawn = LocalPawn();
				if (!pawn) return py::none();
				FVector v = pawn->GetVelocity();
				return py::make_tuple(v.X, v.Y, v.Z); });

			// Point the view at a world position (eye-height origin), instantly. Handy for scripted aim.
			player.def("aim_at", [](float x, float y, float z) -> bool
					   {
				auto* pawn = LocalPawn();
				if (!pawn) return false;
				FVector eye = pawn->K2_GetActorLocation();
				eye.Z += 80.f; // rough eye height
				const float dx = x - eye.X, dy = y - eye.Y, dz = z - eye.Z;
				constexpr float toDeg = 180.f / 3.14159265f;
				FRotator r;
				r.Yaw = std::atan2(dy, dx) * toDeg;
				r.Pitch = std::atan2(dz, std::sqrt(dx * dx + dy * dy)) * toDeg;
				r.Roll = 0.f;
				Engine::PlayerController->SetControlRotation(r);
				return true; }, py::arg("x"), py::arg("y"), py::arg("z"));

			player.def("console", [](std::string command)
					   {
				if (Engine::PlayerController) Engine::PlayerController->SendToConsole(FString(command)); }, py::arg("command"));

			player.def("chat", [](std::string message)
					   {
				if (Engine::PlayerController) Engine::PlayerController->SendChatMessage(FString(message)); }, py::arg("message"));

			player.def("respawn", []
					   {
				auto* c = LocalCharacter();
				if (c) c->RequestSuicide(); }, "Kill the local character so it respawns.");
		}
	} // namespace Modules
} // namespace Scripts
