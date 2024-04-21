#pragma once

#include "../Feature.h"
#include "../../utils/Globals.h"

class SpinBot : public Feature
{
private:
	APortalWarsCharacter* Player = 0;
	float CurrentSpinYaw = 0.f;

	FHitResult resultPlayer;
	FHitResult resultWeapon;
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
		UpdateEnabled();

		if (!Initialized) return false;

		if (!Globals::PlayerController) return false;
		if (!Globals::PlayerController->IsInGame()) return false;

		Player = reinterpret_cast<APortalWarsCharacter*>(Globals::PlayerController->Character);

		if (!Player) return false;
		if (!Player->Mesh1P) return false;

		if (!Player->CurrentWeapon) return false;
		if (!Player->CurrentWeapon->Mesh1P) return false;

		Log(Enabled ? "Enabled" : "Not Enabled");
		return Enabled;
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