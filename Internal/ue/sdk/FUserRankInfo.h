#pragma once

#include "Fwd.h"

// ScriptStruct PortalWarsGlobals.UserRankInfo
// Size: 0x20 (Inherited: 0x00)
struct FUserRankInfo
{
	struct FString RankType;			 // 0x00(0x10)
	int32_t RankLevel;					 // 0x10(0x04)
	int32_t RankXP;						 // 0x14(0x04)
	int32_t PlacementGamesPlayedCount;	 // 0x18(0x04)
	int32_t PlacementGamesPlayedTotal;	 // 0x1c(0x04)
};
