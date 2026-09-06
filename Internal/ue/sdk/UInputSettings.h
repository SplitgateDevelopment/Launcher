#pragma once

#include "Fwd.h"
#include "Values.h"
#include "UObject.h"

// Class Engine.InputSettings
// Size: 0x140 (Inherited: 0x28)
struct UInputSettings : UObject
{
	struct TArray<struct FKey> ConsoleKeys; // 0x130(0x10)

	struct UInputSettings* GetInputSettings(); // Function Engine.InputSettings.GetInputSettings // (Final|Native|Static|Public|BlueprintCallable|BlueprintPure) // @ game+0x3707f30

	static constexpr const char* ClassName = "Class Engine.InputSettings";
};
