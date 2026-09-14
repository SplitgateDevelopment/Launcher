#pragma once

#include "Fwd.h"
#include "Values.h"
#include "UBlueprintFunctionLibrary.h"

// Class Engine.KismetTextLibrary
// Size: 0x28 (Inherited: 0x28)
struct UKismetTextLibrary : UBlueprintFunctionLibrary
{
	struct FText Conv_StringToText(struct FString inString); // Function Engine.KismetTextLibrary.Conv_StringToText // (Final|Native|Static|Public|BlueprintCallable|BlueprintPure) // @ game+0x376b760

	static constexpr const char* ClassName = "Class Engine.KismetTextLibrary";
};
