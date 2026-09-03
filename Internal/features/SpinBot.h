#pragma once

/// @file
/// The SpinBot feature: continuously rotates the local first-person body and
/// weapon meshes about yaw while enabled, resetting them to zero on disable.

#include "Feature.h"
#include "../utils/Globals.h"

/// Spins the local player's first-person meshes by advancing a yaw angle each
/// frame (10 degrees/tick, wrapping at 360).
class SpinBot : public Feature
{
  private:
	APortalWarsCharacter* Player = 0; ///< cached local character; refreshed each Check()
	float CurrentSpinYaw = 0.f;		  ///< accumulated spin angle in degrees; 0 means "at rest"

	FHitResult resultPlayer; ///< out-param sink for the body mesh SetRelativeRotation sweep
	FHitResult resultWeapon; ///< out-param sink for the weapon mesh SetRelativeRotation sweep

  public:
	SpinBot()
	{
		Name = "SpinBot";
		UpdateEnabled();

		Log("Created");
	};

	void UpdateEnabled()
	{
		Enabled = Settings.EXPLOITS.SpinBot;
	};

	bool Check()
	{
		if (!Initialized) return false;

		if (!Globals::PlayerController) return false;
		if (!Globals::PlayerController->IsInGame()) return false;

		Player = reinterpret_cast<APortalWarsCharacter*>(Globals::PlayerController->Character);

		if (!Player) return false;
		if (!Player->Mesh1P) return false;

		if (!Player->CurrentWeapon) return false;
		if (!Player->CurrentWeapon->Mesh1P) return false;

		return true;
	};

	void Init()
	{
		Initialized = true;
		Log("Initialized");
	};

	void Destroy()
	{
		if (CurrentSpinYaw == 0.f) return;

		auto PlayerMesh = Player->Mesh1P;
		auto WeaponMesh = Player->CurrentWeapon->Mesh1P;

		PlayerMesh->K2_SetRelativeRotation(FRotator(0.f, 0.f, 0.f), true, resultPlayer, false);
		WeaponMesh->K2_SetRelativeRotation(FRotator(0.f, 0.f, 0.f), true, resultWeapon, false);

		CurrentSpinYaw = 0.f;
	};

	void Run()
	{
		if (CurrentSpinYaw > 360.f) CurrentSpinYaw = 0.f;

		auto PlayerMesh = Player->Mesh1P;
		auto WeaponMesh = Player->CurrentWeapon->Mesh1P;

		PlayerMesh->K2_SetRelativeRotation(FRotator(0.f, CurrentSpinYaw, 0.f), true, resultPlayer, false);
		WeaponMesh->K2_SetRelativeRotation(FRotator(0.f, CurrentSpinYaw, 0.f), true, resultWeapon, false);

		CurrentSpinYaw += 10.f;
	};
};