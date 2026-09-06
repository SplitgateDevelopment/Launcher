#pragma once

#include "Fwd.h"

// ScriptStruct PortalWarsGlobals.PlayerStatsInfo
// Size: 0x68 (Inherited: 0x00)
struct FPlayerStatsInfo
{
	int32_t Kills;								// 0x00(0x04)
	int32_t Assists;							// 0x04(0x04)
	int32_t Deaths;								// 0x08(0x04)
	int32_t Suicides;							// 0x0c(0x04)
	int32_t HeadshotKills;						// 0x10(0x04)
	int32_t Teabags;							// 0x14(0x04)
	int32_t MeleeKills;							// 0x18(0x04)
	int32_t PortalKills;						// 0x1c(0x04)
	int32_t KillsThruPortal;					// 0x20(0x04)
	int32_t DoubleKills;						// 0x24(0x04)
	int32_t TripleKills;						// 0x28(0x04)
	int32_t QuadKills;							// 0x2c(0x04)
	int32_t QuintKills;							// 0x30(0x04)
	int32_t SexKills;							// 0x34(0x04)
	int32_t Killstreak1;						// 0x38(0x04)
	int32_t Killstreak2;						// 0x3c(0x04)
	int32_t Killstreak3;						// 0x40(0x04)
	int32_t Killstreak4;						// 0x44(0x04)
	int32_t Killstreak5;						// 0x48(0x04)
	int32_t Killstreak6;						// 0x4c(0x04)
	int32_t DamageDealt;						// 0x50(0x04)
	int32_t EmoteCount;							// 0x54(0x04)
	struct TArray<struct FWeaponStats> Weapons; // 0x58(0x10)
};
