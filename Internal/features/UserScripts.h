#pragma once

/// @file
/// The UserScripts feature: bridges the Python scripting layer into the feature
/// loop, ticking every loaded user script once per frame while enabled.

#include "Feature.h"
#include "../scripting/Scripts.h"
#include "../scripting/Events.h"

/// Drives user-supplied Python scripts each frame: runs every script's main()
/// (legacy per-frame model) and dispatches the Render event to those that
/// subscribe via the event bus.
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