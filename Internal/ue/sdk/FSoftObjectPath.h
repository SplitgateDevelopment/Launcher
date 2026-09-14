#pragma once

#include "Fwd.h"

// ScriptStruct CoreUObject.SoftObjectPath
// Size: 0x18 (Inherited: 0x00)
struct FSoftObjectPath
{
	struct FName AssetPathName;	  // 0x00(0x08)
	struct FString SubPathString; // 0x08(0x10)
};
