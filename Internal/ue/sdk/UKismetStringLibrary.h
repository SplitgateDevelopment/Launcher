#pragma once

#include "Fwd.h"
#include "Values.h"
#include "UBlueprintFunctionLibrary.h"

// Class Engine.KismetStringLibrary
// Size: 0x28 (Inherited: 0x28)
struct UKismetStringLibrary : UBlueprintFunctionLibrary
{
	struct FName Conv_StringToName(struct FString inString); // Function Engine.KismetStringLibrary.Conv_StringToName // (Final|Native|Static|Public|BlueprintCallable|BlueprintPure) // @ game+0x374bdb0

	static struct UClass* StaticClass();
};
