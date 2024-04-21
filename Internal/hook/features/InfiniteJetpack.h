#pragma once

#include "../Feature.h"
#include "../../utils/Globals.h"

class InfiniteJetpack : public Feature
{
private:
	APortalWarsCharacter* Player = 0;

	float OriginalThrusterRechargeDelay;
	float OriginalThrusterCurrentTime;
	float OriginalThrusterLoudnessForBots;
public:
	InfiniteJetpack()
	{
		Name = "InfiniteJetpack";
		UpdateEnabled();

		Log("Created");
	};

	void UpdateEnabled()
	{
		Enabled = Settings.EXPLOITS.InfinteJetpack;
	};

	bool Check()
	{
		UpdateEnabled();

		if (!Initialized) return false;

		if (!Globals::PlayerController) return false;
		if (!Globals::PlayerController->IsInGame()) return false;

		Player = reinterpret_cast<APortalWarsCharacter*>(Globals::PlayerController->Character);
		if (!Player) return false;

		Log(Enabled ? "Enabled" : "Not Enabled");
		return Enabled;
	};

	void Init()
	{
		Player = reinterpret_cast<APortalWarsCharacter*>(Globals::PlayerController->Character);

		if (!Player)
		{
			Initialized = false;
			return;
		}

		OriginalThrusterRechargeDelay = Player->thrusterRechargeDelay;
		OriginalThrusterCurrentTime = Player->thrusterCurrentTime;
		OriginalThrusterLoudnessForBots = Player->ThrusterLoudnessForBots;

		Initialized = true;
		Log("Initialized");
	};

	void Destroy()
	{
		if (Player->thrusterRechargeDelay != 0.1f) return;

		Player->thrusterRechargeDelay = OriginalThrusterRechargeDelay;
		Player->thrusterCurrentTime = OriginalThrusterCurrentTime;
		Player->ThrusterLoudnessForBots = OriginalThrusterLoudnessForBots;

		return;
	};

	void Run()
	{
		Player->thrusterRechargeDelay = 0.1f;
		Player->thrusterCurrentTime = 0.0f;
		Player->ThrusterLoudnessForBots = 0.0f;
	};
};