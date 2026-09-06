#pragma once

#include "Fwd.h"
#include "Values.h"
#include "UGameInstance.h"

// Class PortalWars.PortalWarsGameInstance
// Size: 0x638 (Inherited: 0x1a8)
struct UPortalWarsGameInstance : UGameInstance
{
	char pad_1A8[0x8];													 // 0x1a8(0x08)
	char GameSettings[0x1f0];											 // 0x1b0(0x1f0)
	char ForgeMapRepData[0x70];											 // 0x3a0(0x70)
	char pad_410[0x218];												 // 0x410(0x218)
	struct TArray<struct UCheatManagerExtension*> GlobalCheatExtensions; // 0x628(0x10)

	void PreLoadMap(struct FString MapName);																										   // Function PortalWars.PortalWarsGameInstance.PreLoadMap // (Final|Native|Protected) // @ game+0x1688f30
	void OnPostLoadMap(struct UWorld* InLoadedWorld);																								   // Function PortalWars.PortalWarsGameInstance.OnPostLoadMap // (Final|Native|Protected) // @ game+0x1688cf0
	bool IsLevelLoading();																															   // Function PortalWars.PortalWarsGameInstance.IsLevelLoading // (Final|Native|Public|Const) // @ game+0x1688a40
	void HandleTravelFailure(struct UWorld* World, enum class ETravelFailure FailureType, struct FString ErrorString);								   // Function PortalWars.PortalWarsGameInstance.HandleTravelFailure // (Final|Native|Protected) // @ game+0x16888b0
	void HandleNetworkFailure(struct UWorld* World, struct UNetDriver* NetDriver, enum class ENetworkFailure FailureType, struct FString ErrorString); // Function PortalWars.PortalWarsGameInstance.HandleNetworkFailure // (Final|Native|Protected) // @ game+0x1688740
	void DisconnectFromServer();																													   // Function PortalWars.PortalWarsGameInstance.DisconnectFromServer // (Final|Exec|Native|Protected) // @ game+0x1688310
	void Crash();																																	   // Function PortalWars.PortalWarsGameInstance.Crash // (Final|Exec|Native|Protected) // @ game+0x16882d0
};
