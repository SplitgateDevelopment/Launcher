#pragma once

#include "Fwd.h"
#include "FUniqueNetIdWrapper.h"

// ScriptStruct Engine.UniqueNetIdRepl
// Size: 0x28 (Inherited: 0x01)
struct FUniqueNetIdRepl : FUniqueNetIdWrapper
{
	char pad_1[0x17];					  // 0x01(0x17)
	struct TArray<char> ReplicationBytes; // 0x18(0x10)
};
