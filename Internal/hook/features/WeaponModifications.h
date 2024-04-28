#pragma once

#include "../Feature.h"

class WeaponModifications : public Feature
{
private:
	AGun* Weapon = 0;

	uint16_t OriginalTotalAmmos = 0;
	float OriginalMeleeRange = 0.f;
	float OriginalTimeBetweenMelee = 0.f;
	float OriginalMeleeDamage = 0.f;

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
		if (!Globals::PlayerController) return false;
		if (!Globals::PlayerController->IsInGame()) return false;

		auto Player = reinterpret_cast<APortalWarsCharacter*>(Globals::PlayerController->Character);
		if (!Player) return false;

		Weapon = Player->CurrentWeapon;
		if (!Weapon) return false;

		return Enabled;
	};

	void Init()
	{
		auto Player = reinterpret_cast<APortalWarsCharacter*>(Globals::PlayerController->Character);
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

	void Destroy()
	{
	};

	void Run()
	{
		InfiniteAmmo();
		NoReload();
		GodMelee();

		ResetAmmo();
		ResetMelee();
	};

	void InfiniteAmmo()
	{
		if (!Settings.EXPLOITS.InfiniteAmmo) return;
		Weapon->CurrentAmmo = 999;
	};

	void NoReload()
	{
		if (!Settings.EXPLOITS.NoReload) return;
		if (Weapon->CurrentAmmoInClip > 1) return;
		if (Weapon->CurrentAmmo == 0) return;

		int32_t ammoNeeded = Weapon->WeaponConfig.AmmoPerClip - Weapon->CurrentAmmoInClip;

		Weapon->CurrentAmmo -= ammoNeeded > 0 ? ammoNeeded : 0;
		Weapon->CurrentAmmoInClip = Weapon->WeaponConfig.AmmoPerClip;
	};

	void GodMelee()
	{
		if (!Settings.EXPLOITS.GodMelee) return;

		auto Player = reinterpret_cast<APortalWarsCharacter*>(Globals::PlayerController->Character);

		Player->MeleeRange = 999999.f;
		Player->TimeBetweenMelee = 0.1f;
		Weapon->WeaponConfig.MeleeDamage = 999.f;
	};

	void ResetAmmo()
	{
		if (Settings.EXPLOITS.InfiniteAmmo) return;
		if (Weapon->CurrentAmmo < 200) return;

		Weapon->CurrentAmmo = OriginalTotalAmmos;
	};

	void ResetMelee()
	{
		if (Settings.EXPLOITS.GodMelee) return;
		auto Player = reinterpret_cast<APortalWarsCharacter*>(Globals::PlayerController->Character);

		if (Player->MeleeRange < 9999.f) return;

		Player->MeleeRange = OriginalMeleeRange;
		Player->TimeBetweenMelee = OriginalTimeBetweenMelee;
		Weapon->WeaponConfig.MeleeDamage = OriginalMeleeDamage;
	};
};