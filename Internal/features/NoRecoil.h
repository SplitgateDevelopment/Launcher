#pragma once

/// @file
/// The NoRecoil feature: zeroes the current weapon's recoil configuration
/// while enabled, restoring the captured original values on disable.

#include "Feature.h"
#include "../ue/Engine.h"

/// Overwrites the equipped gun's recoilConfig with zeros each frame so shots
/// have no kick or spread.
class NoRecoil : public Feature
{
  private:
	FRecoilData OriginalData; ///< recoil config captured at Init, restored by Destroy()
	AGun* Gun = 0;			  ///< cached current weapon; refreshed each Check()

	/// Copy the six recoil fields from @p inRecoilConfig into @p outRecoilConfig.
	void ApplyRecoil(FRecoilData& outRecoilConfig, FRecoilData inRecoilConfig)
	{
		outRecoilConfig.horizontalRecoilAmount = inRecoilConfig.horizontalRecoilAmount;
		outRecoilConfig.verticalRecoilAmount = inRecoilConfig.verticalRecoilAmount;
		outRecoilConfig.recoilKick = inRecoilConfig.recoilKick;
		outRecoilConfig.visualRecoil = inRecoilConfig.visualRecoil;
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
		if (!Initialized) return false;

		if (!Engine::PlayerController) return false;
		if (!Engine::IsInGame) return false;

		auto Player = reinterpret_cast<APortalWarsCharacter*>(Engine::PlayerController->Character);
		if (!Player) return false;

		Gun = Player->CurrentWeapon;
		if (!Gun) return false;

		return true;
	};

	void Init()
	{
		auto Player = reinterpret_cast<APortalWarsCharacter*>(Engine::PlayerController->Character);

		if (!Player || !Player->CurrentWeapon)
		{
			Initialized = false;
			return;
		}

		ApplyRecoil(OriginalData, Player->CurrentWeapon->recoilConfig);

		Initialized = true;
		Log("Initialized");
	};

	/// Restore the original recoil on disable, but only while our zeroed config
	/// is still in place (recoilKick == 0 marks it as ours).
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
		noRecoilConfig.recoilRiseTime = 0.f;
		noRecoilConfig.recoilTotalTime = 0.f;

		ApplyRecoil(Gun->recoilConfig, noRecoilConfig);
	};
};