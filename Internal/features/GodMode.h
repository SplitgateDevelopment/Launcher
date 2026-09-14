#pragma once

/// @file
/// The GodMode feature: pins the local character's health (and max health) to
/// a very large value while enabled, restoring a normal value on disable.

#include "Feature.h"
#include "../ue/Engine.h"

/// Keeps the local player effectively invulnerable by overwriting Health and
/// MaxHealth every frame while enabled.
class GodMode : public Feature
{
  private:
	float OriginalHealth = 0;		  ///< MaxHealth captured at Init (currently unused by Destroy)
	APortalWarsCharacter* Player = 0; ///< cached local character; refreshed each Check()

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
		if (!Initialized) return false;

		if (!Engine::PlayerController) return false;
		if (!Engine::IsInGame) return false;

		Player = reinterpret_cast<APortalWarsCharacter*>(Engine::PlayerController->Character);

		if (!Player) return false;
		if (!Player->Health) return false;

		return true;
	};

	void Init()
	{
		Player = reinterpret_cast<APortalWarsCharacter*>(Engine::PlayerController->Character);

		if (!Player || !Player->MaxHealth)
		{
			Initialized = false;
			return;
		}

		OriginalHealth = Player->MaxHealth;

		Initialized = true;
		Log("Initialized");
	};

	/// Revert to a normal 100 HP on disable, but only if health still looks
	/// inflated (guards against clobbering a legitimately low value).
	void Destroy()
	{
		if (Player->Health <= 100) return;

		Player->MaxHealth = 100;
		Player->Health = 100;

		return;
	};

	/// Force Health and MaxHealth to 9999 whenever they drift from it.
	void Run()
	{
		float health = 9999;

		if (Player->MaxHealth != health) Player->MaxHealth = health;
		if (Player->Health != health) Player->Health = health;

		// Instigator->healthRechargeDelay = 0.1f;
	};
};