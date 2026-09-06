#pragma once

/// @file
/// The WeaponModifications feature: an always-enabled per-frame pass bundling
/// the weapon exploits (infinite ammo, no-reload, god melee), each gated by its
/// own setting and reverted to captured originals when its setting is off.

#include "Feature.h"

/// Applies the equipped weapon's exploits every tick. Enabled is forced true;
/// each effect is toggled by Settings.EXPLOITS, and the Reset* helpers restore
/// originals in-band (so Destroy() is a no-op).
class WeaponModifications : public Feature
{
  private:
	AGun* Weapon = 0; ///< cached current weapon; refreshed each Check()

	uint16_t OriginalTotalAmmos = 0;	  ///< reserve ammo captured at Init, restored by ResetAmmo()
	float OriginalMeleeRange = 0.f;		  ///< melee range captured at Init, restored by ResetMelee()
	float OriginalTimeBetweenMelee = 0.f; ///< melee cooldown captured at Init, restored by ResetMelee()
	float OriginalMeleeDamage = 0.f;	  ///< melee damage captured at Init, restored by ResetMelee()

  public:
	WeaponModifications()
	{
		Name = "WeaponModifications";
		UpdateEnabled();

		Log("Created");
	};

	void UpdateEnabled()
	{
		Enabled = true;
	};

	bool Check()
	{
		if (!Initialized) return false;
		if (!Engine::PlayerController) return false;
		if (!Engine::IsInGame) return false;

		auto Player = reinterpret_cast<APortalWarsCharacter*>(Engine::PlayerController->Character);
		if (!Player) return false;

		Weapon = Player->CurrentWeapon;
		if (!Weapon) return false;

		return true;
	};

	void Init()
	{
		auto Player = reinterpret_cast<APortalWarsCharacter*>(Engine::PlayerController->Character);
		if (!Player)
		{
			Initialized = false;
			return;
		}

		Weapon = Player->CurrentWeapon;
		if (!Weapon)
		{
			Initialized = false;
			return;
		}

		OriginalTotalAmmos = Weapon->CurrentAmmo;

		OriginalMeleeRange = Player->MeleeRange;
		OriginalTimeBetweenMelee = Player->TimeBetweenMelee;
		OriginalMeleeDamage = Weapon->WeaponConfig.MeleeDamage;

		Initialized = true;
		Log("Initialized");
	};

	void Destroy() {
	};

	/// Apply each enabled weapon exploit, then run the Reset* passes that undo
	/// any exploit whose setting is now off.
	void Run()
	{
		InfiniteAmmo();
		NoReload();
		GodMelee();

		ResetAmmo();
		ResetMelee();
	};

	/// Pin reserve ammo to 999 while InfiniteAmmo is enabled.
	void InfiniteAmmo()
	{
		if (!Settings.EXPLOITS.InfiniteAmmo) return;
		Weapon->CurrentAmmo = 999;
	};

	/// Refill the clip from reserve as soon as it runs low, without a reload
	/// animation (no-op while NoReload is disabled or ammo is unavailable).
	void NoReload()
	{
		if (!Settings.EXPLOITS.NoReload) return;
		if (Weapon->CurrentAmmoInClip > 1) return;
		if (Weapon->CurrentAmmo == 0) return;

		int32_t ammoNeeded = Weapon->WeaponConfig.AmmoPerClip - Weapon->CurrentAmmoInClip;

		Weapon->CurrentAmmo -= ammoNeeded > 0 ? ammoNeeded : 0;
		Weapon->CurrentAmmoInClip = Weapon->WeaponConfig.AmmoPerClip;
	};

	/// Give melee near-infinite range, damage and rate while GodMelee is enabled.
	void GodMelee()
	{
		if (!Settings.EXPLOITS.GodMelee) return;

		auto Player = reinterpret_cast<APortalWarsCharacter*>(Engine::PlayerController->Character);

		Player->MeleeRange = 999999.f;
		Player->TimeBetweenMelee = 0.1f;
		Weapon->WeaponConfig.MeleeDamage = 999.f;
	};

	/// Restore captured reserve ammo once InfiniteAmmo is off, but only if the
	/// value still looks inflated (>= 200) so a real reserve is left untouched.
	void ResetAmmo()
	{
		if (Settings.EXPLOITS.InfiniteAmmo) return;
		if (Weapon->CurrentAmmo < 200) return;

		Weapon->CurrentAmmo = OriginalTotalAmmos;
	};

	/// Restore captured melee stats once GodMelee is off, guarded so a normal
	/// melee range (< 9999) is never overwritten.
	void ResetMelee()
	{
		if (Settings.EXPLOITS.GodMelee) return;
		auto Player = reinterpret_cast<APortalWarsCharacter*>(Engine::PlayerController->Character);

		if (Player->MeleeRange < 9999.f) return;

		Player->MeleeRange = OriginalMeleeRange;
		Player->TimeBetweenMelee = OriginalTimeBetweenMelee;
		Weapon->WeaponConfig.MeleeDamage = OriginalMeleeDamage;
	};
};