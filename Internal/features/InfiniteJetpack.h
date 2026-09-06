#pragma once

/// @file
/// The InfiniteJetpack feature: keeps the local character's thruster/jetpack
/// perpetually charged and silent to bots while enabled, restoring the
/// captured originals on disable.

#include "Feature.h"
#include "../ue/Globals.h"

/// Overwrites the character's thruster recharge/timing fields each frame so the
/// jetpack never depletes.
class InfiniteJetpack : public Feature
{
  private:
	APortalWarsCharacter* Player = 0; ///< cached local character; refreshed each Check()

	float OriginalThrusterRechargeDelay;   ///< captured at Init, restored by Destroy()
	float OriginalThrusterCurrentTime;	   ///< captured at Init, restored by Destroy()
	float OriginalThrusterLoudnessForBots; ///< captured at Init, restored by Destroy()

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

	// Valid to act whenever we have a live local character; the loop decides
	// enabled/disabled and calls Run()/Destroy() accordingly.
	bool Check()
	{
		if (!Initialized) return false;

		if (!Globals::PlayerController) return false;
		if (!Globals::PlayerController->IsInGame()) return false;

		Player = reinterpret_cast<APortalWarsCharacter*>(Globals::PlayerController->Character);
		if (!Player) return false;

		return true;
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

	/// Restore the captured thruster values on disable, but only if they still
	/// hold our injected recharge delay (avoids overwriting the game's own value).
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