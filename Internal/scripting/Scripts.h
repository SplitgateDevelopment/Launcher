#pragma once

#include <iostream>
#include <string>
#include <filesystem>
#include <vector>
#include <fstream>
#include "../utils/Logger.h"
#include "../settings/Settings.h"

#include <Python.h>
#include "../external/pybind11/include/pybind11/embed.h"

#include "modules/Logger.h"
#include "modules/Settings.h"
#include "modules/Events.h"
#include "modules/Actors.h"
#include "modules/Player.h"
#include "modules/Engine.h"
#include "modules/Render.h"
#include "modules/Input.h"

/**
 * @file
 * @brief Embedded CPython scripting host (pybind11).
 *
 * Owns the interpreter and discovers/loads user scripts from the `UserScripts` folder. The
 * `SplitgateInternal` embedded module (defined here) exposes the Logger/Settings/Events
 * submodules to those scripts.
 */

namespace py = pybind11;
namespace fs = std::filesystem;

/// The `SplitgateInternal` module user scripts import; wires up the Logger/Settings/Events
/// submodules on the embedded interpreter.
PYBIND11_EMBEDDED_MODULE(SplitgateInternal, m)
{

	m.doc() = "Splitgate Internal plugin";

	Scripts::Modules::Logger(m);
	Scripts::Modules::Settings(m);
	Scripts::Modules::Events(m);
	Scripts::Modules::Actors(m);
	Scripts::Modules::Player(m);
	Scripts::Modules::Engine(m);
	Scripts::Modules::Render(m);
	Scripts::Modules::Input(m);
}

namespace Scripts
{

	std::vector<std::string> scriptList{};			///< discovered *.py filenames under scriptsPath
	std::vector<pybind11::module_> loadedScripts{}; ///< successfully imported script modules
	fs::path scriptsPath;							///< the UserScripts folder
	py::scoped_interpreter guard{};					///< owns the embedded interpreter for the DLL's lifetime

	/// Resolves the UserScripts folder, discovers every `*.py` (except `__init__.py`), and
	/// imports each into @ref loadedScripts. Creates the folder if missing. Import errors are
	/// logged and skipped, never thrown.
	void Init()
	{
		fs::path scriptsFolder("UserScripts");
		scriptsPath = Shared::AppDataPath(SettingsHelper::AppFolder) / scriptsFolder;

		Logger::Log("INFO", std::format("Loading scripts from {}", scriptsPath.string()));

		if (!fs::exists(scriptsPath) || !fs::is_directory(scriptsPath))
		{
			Logger::Log("ERROR", "UserScripts directory does not exist or is not a valid directory.");
			fs::create_directories(scriptsPath);
			return;
		}

		try
		{
			for (const auto& entry : fs::directory_iterator(scriptsPath))
			{
				fs::path filename = entry.path().filename();

				if (filename.string() != "__init__.py" && filename.string().find(".py") != std::string::npos)
				{
					scriptList.push_back(filename.string());
				}
			}
		}
		catch (const std::exception& e)
		{
			Logger::Log("ERROR", e.what());
		}

		for (int i = 0; i < scriptList.size(); i++)
		{
			try
			{
				std::string filename = scriptList.at(i);
				std::size_t ext = filename.find(".py");

				std::string scriptName = std::string(filename);

				if (ext != std::string::npos)
					scriptName.erase(ext, 3);

				std::string userScriptModule = "UserScripts." + scriptName;
				auto scriptModule = py::module::import(userScriptModule.c_str());

				loadedScripts.push_back(scriptModule);
				Logger::Log("INFO", std::format("UserScript {} loaded", filename));
			}
			catch (py::error_already_set& e)
			{
				Logger::Log("ERROR", e.what());
			}
		}

		Logger::Log("SUCCESS", std::format("Loaded {} scripts", loadedScripts.size()));
	};

	/// Calls `main()` on an already-loaded script. Out-of-range indices and Python errors are
	/// ignored/logged.
	/// @param loadedScriptIndex index into @ref loadedScripts.
	void Execute(int loadedScriptIndex)
	{
		if (loadedScriptIndex >= loadedScripts.size()) return;

		try
		{
			auto script = loadedScripts.at(loadedScriptIndex);
			auto function = script.attr("main");
			function();
		}
		catch (py::error_already_set& e)
		{
			Logger::Log("ERROR", e.what());
		}
	};

	/// Imports a script by filename and calls its `main()` immediately, without adding it to
	/// @ref loadedScripts. Used to run a script that wasn't loaded at startup.
	/// @param filename the script's file name (with or without the `.py` extension).
	void ExecuteUnloaded(std::string filename)
	{
		try
		{
			std::size_t ext = filename.find(".py");

			std::string scriptName = std::string(filename);

			if (ext != std::string::npos)
				scriptName.erase(ext, 3);

			std::string userScriptModule = "UserScripts." + scriptName;
			auto scriptModule = py::module::import(userScriptModule.c_str());

			Logger::Log("INFO", std::format("UserScript {} loaded", filename));

			auto function = scriptModule.attr("main");
			function();

			return;
		}
		catch (py::error_already_set& e)
		{
			Logger::Log("ERROR", e.what());
		}
	}
} // namespace Scripts