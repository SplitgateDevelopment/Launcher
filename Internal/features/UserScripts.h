#pragma once

#include "Feature.h"
#include "../scripting/Scripts.h"
#include "../scripting/Events.h"

class UserScripts : public Feature
{
  public:
	UserScripts()
	{
		Name = "UserScripts";
		UpdateEnabled();

		Log("Created");
	};

	void UpdateEnabled()
	{
		Enabled = Settings.MISC.UserScriptsEnabled;
	};

	bool Check()
	{
		return Initialized;
	};

	void Init()
	{
		Initialized = true;
		Log("Initialized");
	};

	void Destroy() {
	};

	void Run()
	{
		// Legacy per-frame model: every script's main() runs each frame.
		for (int i = 0; i < Scripts::scriptList.size(); i++)
		{
			Scripts::Execute(i);
		}

		// Event model: scripts subscribed to Render run each frame too.
		Events::Dispatch(Events::Type::Render);
	};
};