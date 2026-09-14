#pragma once

#include "Fwd.h"
#include "FPlayerStatsInfo.h"

// ScriptStruct PortalWarsGlobals.PlayerStatsInfo_InDepth
// Size: 0xc8 (Inherited: 0x68)
struct FPlayerStatsInfo_InDepth : FPlayerStatsInfo
{
	int32_t EnemyPortalsDestroyed;	 // 0x68(0x04)
	int32_t AllyPortalsEntered;		 // 0x6c(0x04)
	int32_t EnemyPortalsEntered;	 // 0x70(0x04)
	int32_t OwnPortalsEntered;		 // 0x74(0x04)
	int32_t PortalsSpawned;			 // 0x78(0x04)
	int32_t DistancePortaled;		 // 0x7c(0x04)
	int32_t HighestConsecutiveKills; // 0x80(0x04)
	int32_t HillsCaptured;			 // 0x84(0x04)
	int32_t HillsNeutralized;		 // 0x88(0x04)
	int32_t KillsOnHill;			 // 0x8c(0x04)
	int32_t EnemyKillsOnHill;		 // 0x90(0x04)
	int32_t FlagsPickedUp;			 // 0x94(0x04)
	int32_t FlagsReturned;			 // 0x98(0x04)
	int32_t FlagKills;				 // 0x9c(0x04)
	int32_t FlagCarrierKills;		 // 0xa0(0x04)
	int32_t OddballsPickedUp;		 // 0xa4(0x04)
	int32_t OddballKills;			 // 0xa8(0x04)
	int32_t OddballCarrierKills;	 // 0xac(0x04)
	int32_t VipKills;				 // 0xb0(0x04)
	int32_t KillsAsVIP;				 // 0xb4(0x04)
	int32_t TeabagsDenied;			 // 0xb8(0x04)
	int32_t FirstBloods;			 // 0xbc(0x04)
	int32_t RevengeKills;			 // 0xc0(0x04)
	int32_t KingSlayers;			 // 0xc4(0x04)
};
