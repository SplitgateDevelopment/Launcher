#pragma once

#include "Fwd.h"

struct FMinimalViewInfo
{
	struct FVector Location;					// 0x00(0x0c)
	struct FRotator Rotation;					// 0x0c(0x0c)
	float FOV;									// 0x18(0x04)
	float DesiredFOV;							// 0x1c(0x04)
	float OrthoWidth;							// 0x20(0x04)
	float OrthoNearClipPlane;					// 0x24(0x04)
	float OrthoFarClipPlane;					// 0x28(0x04)
	float AspectRatio;							// 0x2c(0x04)
	char bConstrainAspectRatio : 1;				// 0x30(0x01)
	char bUseFieldOfViewForLOD : 1;				// 0x30(0x01)
	char pad_30_2 : 6;							// 0x30(0x01)
	char pad_31[0x3];							// 0x31(0x03)
	char ProjectionMode;						// 0x34(0x01)
	char pad_35[0x3];							// 0x35(0x03)
	float PostProcessBlendWeight;				// 0x38(0x04)
	char pad_3C[0x4];							// 0x3c(0x04)
	char PostProcessSettings[0x560];			// 0x40(0x560)
	struct FVector2D OffCenterProjectionOffset; // 0x5a0(0x08)
	char pad_5A8[0x48];							// 0x5a8(0x48)
};
