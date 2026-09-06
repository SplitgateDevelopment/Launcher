#pragma once

#include "Fwd.h"

// ScriptStruct PortalWars.AutoAimData
// Size: 0x34 (Inherited: 0x00)
struct FAutoAimData
{
	float AutoAimRadius;					 // 0x00(0x04)
	float AutoAimRadiusZoomed;				 // 0x04(0x04)
	float AutoAimMaxRadiusMultiplier;		 // 0x08(0x04)
	float AutoAimMaxRadiusAutoScale;		 // 0x0c(0x04)
	float AutoAimMinRadiusRange;			 // 0x10(0x04)
	float AutoAimMaxRadiusRange;			 // 0x14(0x04)
	float AutoAimThruPortalRadiusMultiplier; // 0x18(0x04)
	float AutoAimRange;						 // 0x1c(0x04)
	float MagnetismRange;					 // 0x20(0x04)
	float MagnetismAngle;					 // 0x24(0x04)
	bool ShouldUseMagnetism;				 // 0x28(0x01)
	bool bDynamicAutoAimRadius;				 // 0x29(0x01)
	bool bZoomAimAssistOnly;				 // 0x2a(0x01)
	char pad_2B[0x1];						 // 0x2b(0x01)
	float OnTargetTurnRate;					 // 0x2c(0x04)
	float OnTargetTurnRateConsole;			 // 0x30(0x04)
};
