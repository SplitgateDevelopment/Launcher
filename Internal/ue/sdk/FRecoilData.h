#pragma once

#include "Fwd.h"

// ScriptStruct PortalWars.RecoilData
// Size: 0x18 (Inherited: 0x00)
struct FRecoilData
{
	float recoilRiseTime;		  // 0x00(0x04)
	float recoilTotalTime;		  // 0x04(0x04)
	float verticalRecoilAmount;	  // 0x08(0x04)
	float horizontalRecoilAmount; // 0x0c(0x04)
	float recoilKick;			  // 0x10(0x04)
	float visualRecoil;			  // 0x14(0x04)
};
