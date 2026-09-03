#pragma once

#include "../settings/Settings.h"
#include "../utils/Logger.h"

class Feature
{
public:
	bool Enabled = false;
	bool Initialized = false;
	std::string Name = "BaseFeature";

	Feature() {};

	virtual void Init() = 0;
	virtual void UpdateEnabled() = 0;
	virtual bool Check() = 0;
	virtual void Destroy() = 0;
	virtual void Run() = 0;

	void Log(std::string message)
	{
		if (!Settings.DEBUG.FeaturesLogging) return;
		Logger::Log("FEATURE", std::format("[{}] {}", Name, message));
	};
};