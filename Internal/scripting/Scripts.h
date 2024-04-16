#pragma once

#include <iostream>
#include <string>
#include <filesystem>
#include <vector>
#include <fstream>
#include "../utils/Logger.h"
#include "../settings/Settings.h"

#include <Python.h>
#include <pybind11/embed.h>

#include "modules/Logger.h"
#include "modules/Settings.h"

namespace py = pybind11;
namespace fs = std::filesystem;

PYBIND11_EMBEDDED_MODULE(SplitgateInternal, m) {

	m.doc() = "Splitgate Internal plugin";

	Scripts::Modules::Logger(m);
	Scripts::Modules::Settings(m);
}

namespace Scripts {

	std::vector<std::string> scriptList{};
	std::vector<pybind11::module_> loadedScripts{};
	fs::path scriptsPath;
	py::scoped_interpreter guard{};

	void Init()
	{
		fs::path scriptsFolder("UserScripts");
		scriptsPath = SettingsHelper::GetAppPath() / scriptsFolder;

		Logger::Log("INFO", std::format("Loading scripts from {}", scriptsPath.string()));

		if (!fs::exists(scriptsPath) || !fs::is_directory(scriptsPath)) {
			Logger::Log("ERROR", "UserScripts directory does not exist or is not a valid directory.");
			fs::create_directories(scriptsPath);
			return;
		}

		try
		{
			for (const auto& entry : fs::directory_iterator(scriptsPath)) {
				fs::path filename = entry.path().filename();

				if (filename.string() != "__init__.py" && filename.string().find(".py") != std::string::npos) {
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
			try {
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
			catch (py::error_already_set& e) {
				Logger::Log("ERROR", e.what());
			}
		}

		Logger::Log("SUCCESS", std::format("Loaded {} scripts", loadedScripts.size()));
	};

	void Execute(int loadedScriptIndex)
	{
		if (loadedScriptIndex >= loadedScripts.size()) return;

		try
		{
			auto script = loadedScripts.at(loadedScriptIndex);
			auto function = script.attr("main");
			function();
		}
		catch (py::error_already_set& e) {
			Logger::Log("ERROR", e.what());
		}
	};
		
	void ExecuteUnloaded(std::string filename)
	{
		try {
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
		catch (py::error_already_set& e) {
			Logger::Log("ERROR", e.what());
		}
	}
}