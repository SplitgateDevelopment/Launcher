#pragma once

#include "../Feature.h"
#include "../../scripting/Scripts.h"

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
		UpdateEnabled();

		if (!Initialized) return false;

		Log(Enabled ? "Enabled" : "Not Enabled");
		return Enabled;
	};

	void Init()
	{
		Initialized = true;
		Log("Initialized");
	};

	void Destroy()
	{
	};

	void Run()
	{
		for (int i = 0; i < Scripts::scriptList.size(); i++)
		{
			Scripts::Execute(i);
		}
	};
};