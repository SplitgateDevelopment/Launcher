#pragma once

#include "Fwd.h"
#include "Values.h"
#include "UScriptViewportClient.h"

// Class Engine.GameViewportClient
// Size: 0x360 (Inherited: 0x38)
struct UGameViewportClient : UScriptViewportClient
{
	char pad_38[0x8];											 // 0x38(0x08)
	struct UConsole* ViewportConsole;							 // 0x40(0x08)
	struct TArray<struct FDebugDisplayProperty> DebugProperties; // 0x48(0x10)
	char pad_58[0x10];											 // 0x58(0x10)
	int32_t MaxSplitscreenPlayers;								 // 0x68(0x04)
	char pad_6C[0xc];											 // 0x6c(0x0c)
	struct UWorld* World;										 // 0x78(0x08)
	struct UGameInstance* GameInstance;							 // 0x80(0x08)
	char pad_88[0x2d8];											 // 0x88(0x2d8)

	void SSSwapControllers();					// Function Engine.GameViewportClient.SSSwapControllers // (Exec|Native|Public) // @ game+0x102e110
	void ShowTitleSafeArea();					// Function Engine.GameViewportClient.ShowTitleSafeArea // (Exec|Native|Public) // @ game+0x27d8180
	void SetConsoleTarget(int32_t PlayerIndex); // Function Engine.GameViewportClient.SetConsoleTarget // (Exec|Native|Public) // @ game+0x37031e0
};
