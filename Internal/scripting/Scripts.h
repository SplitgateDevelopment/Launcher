#pragma once

#include <iostream>
#include <string>
#include <filesystem>
#include <vector>
#include <fstream>

#include <Python.h>
#include <pybind11/embed.h>
#include "../utils/Logger.h"
#include "../settings/Settings.h"

namespace py = pybind11;
namespace fs = std::filesystem;

PYBIND11_EMBEDDED_MODULE(SplitgateInternal, m) {

	m.doc() = "Splitgate Internal plugin";

	auto logger = m.def_submodule("Logger");
	logger.def("Log", &Logger::Log);

	auto settings = m.def_submodule("Settings");
	settings.def("Reset", &SettingsHelper::Reset);
	settings.def("Save", &SettingsHelper::Save);
}

namespace Scripts {

	std::vector<std::string> scriptList{};
	std::vector<pybind11::module_> loadedScripts{};
	std::string scriptsPath;
	py::scoped_interpreter guard{};

	void Init()
	{
		fs::path path = fs::current_path();
		fs::path file("UserScripts");
		fs::path full_path = path / file;
		scriptsPath = full_path.string();

		if (!fs::exists(full_path) || !fs::is_directory(full_path)) {
			Logger::Log("ERROR", "UserScripts directory does not exist or is not a valid directory.");
			return;
		}

		try
		{
			for (const auto& entry : fs::directory_iterator(full_path)) {
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

		for (int i = 0; i < scriptList.size(); i++) //Iterate through valid scripts
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

		Logger::Log("SUCCESS", std::format("Loaded {} scripts", scriptList.size()));
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