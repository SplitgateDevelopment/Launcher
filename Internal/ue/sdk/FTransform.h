#pragma once

#include "Fwd.h"

struct FTransform
{
	FQuat Rotation;			 // 0x00
	struct FVector Translation; // 0x10
	char pad_1C[0x4];		 // 0x1C
	struct FVector Scale3D;	 // 0x20
	char pad_2C[0x4];		 // 0x2C
};
