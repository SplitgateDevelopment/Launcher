#pragma once

#include <cstdint>
#include <format>
#include <string>
#include <vector>

#include <pybind11/embed.h>
#include <pybind11/stl.h>

#include "../../ue/Engine.h"
#include "../../utils/Globals.h"
#include "../../cache/ActorCache.h"

/**
 * @file
 * @brief pybind11 module exposing the world's player characters to user scripts as
 * `SplitgateInternal.Actors.players()` — a frame snapshot (location/team/health/name/is_local),
 * read from the shared ActorCache so scripts share the one per-frame pass.
 */

namespace py = pybind11;

namespace Scripts
{
	namespace Modules
	{
		/// A per-frame snapshot of one character. Plain values (no raw pointers held past the frame);
		/// @ref address is the object pointer as an int for advanced use.
		struct PlayerInfo
		{
			double x = 0, y = 0, z = 0;
			int team = -1;
			float health = 0.f, maxHealth = 0.f;
			std::string name;
			bool isLocal = false;
			std::uintptr_t address = 0;
		};

		/// Rebuild the actor list (unconditionally, so scripts see it regardless of the ESP/aim
		/// toggles) and snapshot every character into plain values.
		inline std::vector<PlayerInfo> CollectPlayers()
		{
			std::vector<PlayerInfo> out;
			if (!Globals::PlayerController) return out;

			ActorCache::Rebuild();
			auto* localPawn = reinterpret_cast<AActor*>(Globals::PlayerController->AcknowledgedPawn);

			for (const auto& p : ActorCache::Players())
			{
				PlayerInfo info;
				info.x = p.location.X;
				info.y = p.location.Y;
				info.z = p.location.Z;
				info.team = p.team;
				info.health = p.health;
				info.maxHealth = p.character ? p.character->MaxHealth : 0.f;
				info.isLocal = (reinterpret_cast<AActor*>(p.character) == localPawn);
				info.address = reinterpret_cast<std::uintptr_t>(p.character);
				if (p.character && p.character->PlayerState)
					info.name = p.character->PlayerState->PlayerNamePrivate.ToString();
				out.push_back(std::move(info));
			}
			return out;
		}

		/// Registers the `Actors` submodule: a `Player` snapshot type plus `players()` / `enemies()` /
		/// `count()`.
		void Actors(py::module_& m)
		{
			auto actors = m.def_submodule("Actors");

			py::class_<PlayerInfo>(actors, "Player")
				.def_readonly("x", &PlayerInfo::x)
				.def_readonly("y", &PlayerInfo::y)
				.def_readonly("z", &PlayerInfo::z)
				.def_property_readonly("location", [](const PlayerInfo& p)
									   { return py::make_tuple(p.x, p.y, p.z); })
				.def_readonly("team", &PlayerInfo::team)
				.def_readonly("health", &PlayerInfo::health)
				.def_readonly("max_health", &PlayerInfo::maxHealth)
				.def_readonly("name", &PlayerInfo::name)
				.def_readonly("is_local", &PlayerInfo::isLocal)
				.def_readonly("address", &PlayerInfo::address)
				.def("__repr__", [](const PlayerInfo& p)
					 { return std::format("<Player {} team={} hp={:.0f} local={}>", p.name, p.team, p.health, p.isLocal); });

			actors.def("players", &CollectPlayers, "Every PortalWarsCharacter this frame (includes the local player).");
			actors.def("count", []
					   { return CollectPlayers().size(); });

			// Enemies only: skip the local player and anyone on the local team (team unknown -> keep).
			actors.def("enemies", []
					   {
				auto all = CollectPlayers();
				int localTeam = -1;
				for (const auto& p : all) if (p.isLocal) { localTeam = p.team; break; }
				std::vector<PlayerInfo> out;
				for (auto& p : all)
					if (!p.isLocal && (localTeam < 0 || p.team != localTeam)) out.push_back(std::move(p));
				return out; });
		}
	} // namespace Modules
} // namespace Scripts
