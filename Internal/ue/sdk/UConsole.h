#pragma once

#include "Fwd.h"
#include "Values.h"
#include "UObject.h"

// Class Engine.Console
// Size: 0x130 (Inherited: 0x28)
struct UConsole : UObject
{
	char pad_28[0x10];							 // 0x28(0x10)
	struct ULocalPlayer* ConsoleTargetPlayer;	 // 0x38(0x08)
	struct UTexture2D* DefaultTexture_Black;	 // 0x40(0x08)
	struct UTexture2D* DefaultTexture_White;	 // 0x48(0x08)
	char pad_50[0x18];							 // 0x50(0x18)
	struct TArray<struct FString> HistoryBuffer; // 0x68(0x10)
	char pad_78[0xb8];							 // 0x78(0xb8)

	static struct UClass* StaticClass();
};
