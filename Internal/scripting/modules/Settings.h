#pragma once

#include <string>

#include <pybind11/embed.h>
#include "../../settings/Settings.h"
#include "../Events.h"

/**
 * @file
 * @brief pybind11 module exposing settings to user scripts as `SplitgateInternal.Settings.*`:
 * `Reset()` / `Save()`, plus a generic JSON-backed `get(path)` / `set(path, value)` /
 * `toggle(path)` that works for *every* setting (current and future) via the same
 * nlohmann serialization the config file uses. Paths are dot-separated, e.g. "VISUALS.Esp".
 */

namespace py = pybind11;

namespace Scripts
{
	namespace Modules
	{
		/// "VISUALS.Esp" -> a JSON pointer "/VISUALS/Esp".
		inline nlohmann::json::json_pointer ToPointer(std::string path)
		{
			for (char& ch : path)
				if (ch == '.') ch = '/';
			if (path.empty() || path.front() != '/') path.insert(path.begin(), '/');
			return nlohmann::json::json_pointer(path);
		}

		/// Convert a JSON scalar to a Python object (objects/arrays come back as their JSON string).
		inline py::object JsonToPy(const nlohmann::json& j)
		{
			if (j.is_boolean()) return py::bool_(j.get<bool>());
			if (j.is_number_float()) return py::float_(j.get<double>());
			if (j.is_number_unsigned()) return py::int_(j.get<unsigned long long>());
			if (j.is_number_integer()) return py::int_(j.get<long long>());
			if (j.is_string()) return py::str(j.get<std::string>());
			if (j.is_null()) return py::none();
			return py::str(j.dump());
		}

		/// Convert a Python scalar to JSON (bool checked before int, since Python bool is an int).
		inline nlohmann::json PyToJson(const py::handle& o)
		{
			if (py::isinstance<py::bool_>(o)) return o.cast<bool>();
			if (py::isinstance<py::int_>(o)) return o.cast<long long>();
			if (py::isinstance<py::float_>(o)) return o.cast<double>();
			if (py::isinstance<py::str>(o)) return o.cast<std::string>();
			return nlohmann::json();
		}

		/// Registers the `Settings` submodule.
		void Settings(py::module_& m)
		{
			auto settings = m.def_submodule("Settings");

			settings.def("Reset", []
						 { SettingsHelper::File().Reset(); });
			settings.def("Save", []
						 { SettingsHelper::File().Save(); });

			// Read any setting by dotted path (e.g. "AIM.AimFov"); None if the path doesn't exist.
			settings.def("get", [](std::string path) -> py::object
						 {
				nlohmann::json j = ::Settings;
				auto ptr = ToPointer(path);
				if (!j.contains(ptr)) return py::none();
				return JsonToPy(j.at(ptr)); }, py::arg("path"));

			// Write any setting by dotted path, then dispatch SettingsChanged (features refresh,
			// autosave fires) — the same path the menu uses.
			settings.def("set", [](std::string path, py::object value) -> bool
						 {
				nlohmann::json j = ::Settings;
				auto ptr = ToPointer(path);
				if (!j.contains(ptr)) return false;
				j[ptr] = PyToJson(value);
				::Settings = j.get<SETTINGS>();
				::Events::Dispatch(::Events::Type::SettingsChanged);
				return true; }, py::arg("path"), py::arg("value"));

			// Flip a boolean setting; returns the new value, or None if the path isn't a bool.
			settings.def("toggle", [](std::string path) -> py::object
						 {
				nlohmann::json j = ::Settings;
				auto ptr = ToPointer(path);
				if (!j.contains(ptr) || !j.at(ptr).is_boolean()) return py::none();
				const bool nv = !j.at(ptr).get<bool>();
				j[ptr] = nv;
				::Settings = j.get<SETTINGS>();
				::Events::Dispatch(::Events::Type::SettingsChanged);
				return py::bool_(nv); }, py::arg("path"));
		}
	} // namespace Modules
} // namespace Scripts
