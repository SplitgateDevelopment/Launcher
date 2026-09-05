#pragma once

#include <cmath>
#include <cstdint>
#include <format>
#include <string>
#include <vector>

#include <pybind11/embed.h>
#include <pybind11/stl.h>

#include "../../ue/Engine.h"
#include "../../utils/Globals.h"
#include "../../utils/Visibility.h"
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
			bool isBot = false;
			int kills = 0, deaths = 0, killstreak = 0;
			std::uintptr_t address = 0;
		};

		/// Validate an address against the *current* actor cache and return the live character (or
		/// nullptr). Used by the per-player methods so a stale/dangling address can't be dereferenced —
		/// call `players()` first so the cache holds this frame's pointers.
		inline APortalWarsCharacter* ResolveLive(std::uintptr_t address)
		{
			for (const auto& p : ActorCache::Players())
				if (reinterpret_cast<std::uintptr_t>(p.character) == address) return p.character;
			return nullptr;
		}

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
				info.isBot = p.isBot;
				info.kills = p.kills;
				info.deaths = p.deaths;
				info.killstreak = p.killstreak;
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
				.def_readonly("is_bot", &PlayerInfo::isBot)
				.def_readonly("kills", &PlayerInfo::kills)
				.def_readonly("deaths", &PlayerInfo::deaths)
				.def_readonly("killstreak", &PlayerInfo::killstreak)
				.def_readonly("address", &PlayerInfo::address)
				// Live per-player queries (re-validate the address against the current cache first, so a
				// stale snapshot can't crash the game). Call players()/enemies() this frame first.
				.def("bone", [](const PlayerInfo& p, BoneFNames index) -> py::object
					 {
					auto* c = ResolveLive(p.address);
					if (!c || !c->Mesh) return py::none();
					FVector v = c->Mesh->GetBoneMatrix(index);
					return py::make_tuple(v.X, v.Y, v.Z); }, py::arg("index"), "World position of a bone (Actors.Bone.*), or None.")
				.def("visible", [](const PlayerInfo& p)
					 {
					auto* c = ResolveLive(p.address);
					return c && Visibility::IsVisible(c, 0.1f); }, "Whether the character is currently visible (trace-free render-timestamp check).")
				.def("distance", [](const PlayerInfo& p) -> py::object
					 {
					if (!Globals::PlayerController || !Globals::PlayerController->AcknowledgedPawn) return py::none();
					FVector me = reinterpret_cast<AActor*>(Globals::PlayerController->AcknowledgedPawn)->K2_GetActorLocation();
					const double dx = p.x - me.X, dy = p.y - me.Y, dz = p.z - me.Z;
					return py::float_(std::sqrt(dx * dx + dy * dy + dz * dz) / 100.0); }, "Distance from the local player, in metres.")
				.def("__repr__", [](const PlayerInfo& p)
					 { return std::format("<Player {} team={} hp={:.0f} local={}>", p.name, p.team, p.health, p.isLocal); });

			// Common bone indices (BoneFNames) for bone()/skeleton drawing.
			py::enum_<BoneFNames>(actors, "Bone")
				.value("Root", BoneFNames::Root)
				.value("Pelvis", BoneFNames::pelvis)
				.value("Spine01", BoneFNames::spine_01)
				.value("Spine03", BoneFNames::spine_03)
				.value("Neck", BoneFNames::neck_01)
				.value("Head", BoneFNames::head)
				.value("UpperArmL", BoneFNames::upperarm_l)
				.value("LowerArmL", BoneFNames::lowerarm_l)
				.value("HandL", BoneFNames::hand_l)
				.value("UpperArmR", BoneFNames::upperarm_r)
				.value("LowerArmR", BoneFNames::lowerarm_r)
				.value("HandR", BoneFNames::hand_r)
				.value("ThighL", BoneFNames::thigh_l)
				.value("CalfL", BoneFNames::calf_l)
				.value("FootL", BoneFNames::foot_l)
				.value("ThighR", BoneFNames::thigh_r)
				.value("CalfR", BoneFNames::calf_r)
				.value("FootR", BoneFNames::foot_r)
				.export_values();

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
