#pragma once

#include "Fwd.h"
#include "Values.h"
#include "ACullableActor.h"

// Class PortalWars.BaseGun
// Size: 0x2c0 (Inherited: 0x220)
struct ABaseGun : ACullableActor
{
	char pad_220[0x10];							   // 0x220(0x10)
	struct UParticleSystem* MuzzleFX;			   // 0x230(0x08)
	struct FVector Muzzle1pScale;				   // 0x238(0x0c)
	struct FVector Muzzle3pScale;				   // 0x244(0x0c)
	struct UParticleSystemComponent* MuzzlePSC_1P; // 0x250(0x08)
	struct UParticleSystemComponent* MuzzlePSC_3P; // 0x258(0x08)
	char bLoopedMuzzleFX : 1;					   // 0x260(0x01)
	char pad_260_1 : 7;							   // 0x260(0x01)
	char pad_261[0x7];							   // 0x261(0x07)
	struct APortalWarsCharacter* MyPawn;		   // 0x268(0x08)
	struct USkeletalMeshComponent* Mesh1P;		   // 0x270(0x08)
	struct UStaticMeshComponent* Mesh3P;		   // 0x278(0x08)
	char SkinAssetType[0x08];					   // 0x280(0x08)
	char pad_288[0x18];							   // 0x288(0x18)
	struct ABaseGunSkin* DefaultWeaponSkinClass;   // 0x2a0(0x08)
	struct ABaseGunSkin* WeaponSkin;			   // 0x2a8(0x08)
	struct ABaseGunSkin* WeaponSkinClass;		   // 0x2b0(0x08)
	float LoudnessForBots;						   // 0x2b8(0x04)
	char pad_2BC[0x4];							   // 0x2bc(0x04)

	void UpdateSkins();							// Function PortalWars.BaseGun.UpdateSkins // (Native|Protected) // @ game+0x8f5850
	struct USkeletalMeshComponent* GetMesh1P(); // Function PortalWars.BaseGun.GetMesh1P // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x164ce50
	int32_t GetControllingTeam();				// Function PortalWars.BaseGun.GetControllingTeam // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x164ce20
	void ClientRemoved();						// Function PortalWars.BaseGun.ClientRemoved // (Net|NetReliableNative|Event|Protected|NetClient) // @ game+0x164ce00
};
