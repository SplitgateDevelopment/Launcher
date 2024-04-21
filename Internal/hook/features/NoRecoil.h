#pragma once

#include "../Feature.h"
#include "../../utils/Globals.h"

class NoRecoil : public Feature
{
private:
	FRecoilData OriginalData;
	AGun* Gun = 0;

	void ApplyRecoil(FRecoilData &outRecoilConfig, FRecoilData inRecoilConfig)
	{
		outRecoilConfig.horizontalRecoilAmount = inRecoilConfig.horizontalRecoilAmount;
		outRecoilConfig.verticalRecoilAmount = inRecoilConfig.verticalRecoilAmount;
		outRecoilConfig.recoilKick = inRecoilConfig.recoilKick;
		outRecoilConfig.visualRecoil = inRecoilConfig.visualRecoil;
		outRecoilConfig.recoilKick = inRecoilConfig.recoilKick;
		outRecoilConfig.recoilRiseTime = inRecoilConfig.recoilRiseTime;
		outRecoilConfig.recoilTotalTime = inRecoilConfig.recoilTotalTime;
	};
public:
	NoRecoil()
	{
		Name = "NoRecoil";
		UpdateEnabled();

		Log("Created");
	};

	void UpdateEnabled()
	{
		Enabled = Settings.EXPLOITS.NoRecoil;
	};

	bool Check()
	{
		UpdateEnabled();

		if (!Initialized) return false;

		if (!Globals::PlayerController) return false;
		if (!Globals::PlayerController->IsInGame()) return false;

		auto Player = reinterpret_cast<APortalWarsCharacter*>(Globals::PlayerController->Character);
		if (!Player) return false;

		Gun = Player->CurrentWeapon;
		if (!Gun) return false;

		Log(Enabled ? "Enabled" : "Not Enabled");
		return Enabled;
	};

	void Init()
	{
		auto Player = reinterpret_cast<APortalWarsCharacter*>(Globals::PlayerController->Character);

		if (!Player || !Player->CurrentWeapon)
		{
			Initialized = false;
			return;
		}

		ApplyRecoil(OriginalData, Player->CurrentWeapon->recoilConfig);

		Initialized = true;
		Log("Initialized");
	};

	void Destroy()
	{
		if (Gun->recoilConfig.recoilKick != 0.f) return;

		ApplyRecoil(Gun->recoilConfig, OriginalData);

		return;
	};

	void Run()
	{
		FRecoilData noRecoilConfig;
		noRecoilConfig.horizontalRecoilAmount = 0.f;
		noRecoilConfig.verticalRecoilAmount = 0.f;
		noRecoilConfig.recoilKick = 0.f;
		noRecoilConfig.visualRecoil = 0.f;
		noRecoilConfig.recoilKick = 0.f;
		noRecoilConfig.recoilRiseTime = 0.f;
		noRecoilConfig.recoilTotalTime = 0.f;

		ApplyRecoil(Gun->recoilConfig, noRecoilConfig);
	};
};