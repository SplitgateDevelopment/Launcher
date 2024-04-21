#pragma once

#include "../Feature.h"
#include "../../utils/Globals.h"

class PlayerModifications : public Feature
{
public:
	PlayerModifications()
	{
		Name = "PlayerModifications";
		UpdateEnabled();

		Log("Created");
	};

	void UpdateEnabled()
	{
		Enabled = true;
	};

	bool Check()
	{
		UpdateEnabled();

		if (!Initialized) return false;
		if (!Globals::PlayerController) return false;

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
		Globals::PlayerController->SetName(FString((Settings.MISC.PlayerName)));
		Globals::PlayerController->FOV(Settings.EXPLOITS.FOV);

		if (!Globals::PlayerController->IsInGame()) return;

		auto Player = reinterpret_cast<APortalWarsCharacter*>(Globals::PlayerController->Character);
		Player->CustomTimeDilation = Settings.EXPLOITS.PlayerSpeed;
	};
};