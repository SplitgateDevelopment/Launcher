#pragma once

#include "Fwd.h"

// ScriptStruct Engine.TViewTarget
// Size: 0x610 (Inherited: 0x00)
struct FTViewTarget
{
	struct AActor* Target;			  // 0x00(0x08)
	char pad_8[0x8];				  // 0x08(0x08)
	struct FMinimalViewInfo POV;	  // 0x10(0x5f0)
	struct APlayerState* PlayerState; // 0x600(0x08)
	char pad_608[0x8];				  // 0x608(0x08)
};
