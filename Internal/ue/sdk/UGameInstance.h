#pragma once

#include "Fwd.h"
#include "Values.h"
#include "UObject.h"

// Class Engine.GameInstance
// Size: 0x1a8 (Inherited: 0x28)
struct UGameInstance : UObject
{
	char pad_28[0x10];												  // 0x28(0x10)
	struct TArray<struct ULocalPlayer*> LocalPlayers;				  // 0x38(0x10)
	struct UOnlineSession* OnlineSession;							  // 0x48(0x08)
	struct TArray<struct UObject*> ReferencedObjects;				  // 0x50(0x10)
	char pad_60[0x18];												  // 0x60(0x18)
	struct FMulticastInlineDelegate OnPawnControllerChangedDelegates; // 0x78(0x10)
	char pad_88[0x120];												  // 0x88(0x120)

	void ReceiveShutdown();															 // Function Engine.GameInstance.ReceiveShutdown // (Event|Public|BlueprintEvent) // @ game+0x1a5c6b0
	void ReceiveInit();																 // Function Engine.GameInstance.ReceiveInit // (Event|Public|BlueprintEvent) // @ game+0x1a5c6b0
	void HandleTravelError(enum class ETravelFailure FailureType);					 // Function Engine.GameInstance.HandleTravelError // (Event|Public|BlueprintEvent) // @ game+0x1a5c6b0
	void HandleNetworkError(enum class ENetworkFailure FailureType, bool bIsServer); // Function Engine.GameInstance.HandleNetworkError // (Event|Public|BlueprintEvent) // @ game+0x1a5c6b0
	void DebugRemovePlayer(int32_t ControllerId);									 // Function Engine.GameInstance.DebugRemovePlayer // (Exec|Native|Public) // @ game+0x36e9fc0
	void DebugCreatePlayer(int32_t ControllerId);									 // Function Engine.GameInstance.DebugCreatePlayer // (Exec|Native|Public) // @ game+0x36e9f30
};
