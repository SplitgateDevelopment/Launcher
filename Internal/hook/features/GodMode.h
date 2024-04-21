#pragma once

#include "../Feature.h"
#include "../../utils/Globals.h"

class GodMode : public Feature
{
private:
		float OriginalHealth = 0;
		APortalWarsCharacter* Player = 0;
public:
	GodMode()
	{
		Name = "GodMode";
		UpdateEnabled();

		Log("Created");
	};

	void UpdateEnabled()
	{
		Enabled = Settings.EXPLOITS.GodMode;
	};

	bool Check()
	{
		UpdateEnabled();

		if (!Initialized) return false;

		if (!Globals::PlayerController) return false;
		if (!Globals::PlayerController->IsInGame()) return false;

		Player = reinterpret_cast<APortalWarsCharacter*>(Globals::PlayerController->Character);

		if (!Player) return false;
		if (!Player->Health) return false;

		Log(Enabled ? "Enabled" : "Not Enabled");
		return Enabled;
	};

	void Init()
	{
		Player = reinterpret_cast<APortalWarsCharacter*>(Globals::PlayerController->Character);

		if (!Player || !Player->MaxHealth)
		{
			Initialized = false;
			return;
		}

		OriginalHealth = Player->MaxHealth;

		Initialized = true;
		Log("Initialized");
	};

	void Destroy()
	{
		if (Player->Health <= 100) return;

		Player->MaxHealth = 100;
		Player->Health = 100;

		return;
	};

	void Run()
	{
		float health = 9999;

		if (Player->MaxHealth != health) Player->MaxHealth = health;
		if (Player->Health != health) Player->Health = health;

		//Instigator->healthRechargeDelay = 0.1f;
	};
};