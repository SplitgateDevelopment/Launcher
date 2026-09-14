#pragma once

#include "Fwd.h"

// ScriptStruct PortalWars.WeaponData
// Size: 0x2c (Inherited: 0x00)
struct FWeaponData
{
	int32_t MaxAmmo;			   // 0x00(0x04)
	int32_t AmmoPerClip;		   // 0x04(0x04)
	int32_t InitialClips;		   // 0x08(0x04)
	enum class EAmmoType AmmoType; // 0x0c(0x01)
	char pad_D[0x3];			   // 0x0d(0x03)
	float TimeBetweenShots;		   // 0x10(0x04)
	bool bIsSingleShot;			   // 0x14(0x01)
	char pad_15[0x3];			   // 0x15(0x03)
	float NoAnimReloadDuration;	   // 0x18(0x04)
	bool CanZoom;				   // 0x1c(0x01)
	char pad_1D[0x3];			   // 0x1d(0x03)
	float ZoomFOV;				   // 0x20(0x04)
	bool CanSwapForSameWeapon;	   // 0x24(0x01)
	char pad_25[0x3];			   // 0x25(0x03)
	float MeleeDamage;			   // 0x28(0x04)
};
