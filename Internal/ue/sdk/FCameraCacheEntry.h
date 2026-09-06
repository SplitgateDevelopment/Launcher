#pragma once

#include "Fwd.h"

// ScriptStruct Engine.CameraCacheEntry
// Size: 0x600 (Inherited: 0x00)
struct FCameraCacheEntry
{
	float Timestamp;			 // 0x00(0x04)
	char pad_4[0xc];			 // 0x04(0x0c)
	struct FMinimalViewInfo POV; // 0x10(0x5f0)
};
