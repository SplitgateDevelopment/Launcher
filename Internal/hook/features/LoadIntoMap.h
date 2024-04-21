#pragma once

#include "../Feature.h"
#include "../../utils/Globals.h"

class LoadIntoMap : public Feature
{
public:
	LoadIntoMap()
	{
		Name = "LoadIntoMap";
		UpdateEnabled();

		Log("Created");
	};

	void UpdateEnabled()
	{
		Enabled = Settings.MISC.LoadIntoMap;
	};

	bool Check()
	{
		UpdateEnabled();

		if (!Initialized) return false;

		if (!Globals::PlayerController) return false;
		if (Globals::PlayerController->IsInGame()) return false;

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
		//TODO Enable Play Button
		//UPortalWarsPlayButtonWidget:UPortalWarsButtonWidget:UPortalWarsGenericButton:UPortalWarsUserWidget:UUserWidget:UWidget:UVisual:UObject
		//bool IsEnabled(); // Function PortalWars.PortalWarsButtonWidget.IsEnabled // (Native|Public|Const) // @ game+0x1664af0

		Logger::Log("INFO", "Loading into map");
		Globals::PlayerController->SwitchLevel(L"Simulation_Alpha");
		Settings.MISC.LoadIntoMap = false;
	};
};