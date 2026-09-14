#pragma once

#include "Fwd.h"
#include "Values.h"
#include "AInfo.h"

// Class Engine.PlayerState
// Size: 0x320 (Inherited: 0x220)
struct APlayerState : AInfo
{
	float Score;							  // 0x220(0x04)
	int32_t PlayerId;						  // 0x224(0x04)
	char Ping;								  // 0x228(0x01)
	char pad_229[0x1];						  // 0x229(0x01)
	char bShouldUpdateReplicatedPing : 1;	  // 0x22a(0x01)
	char bIsSpectator : 1;					  // 0x22a(0x01)
	char bOnlySpectator : 1;				  // 0x22a(0x01)
	char bIsABot : 1;						  // 0x22a(0x01)
	char pad_22A_4 : 1;						  // 0x22a(0x01)
	char bIsInactive : 1;					  // 0x22a(0x01)
	char bFromPreviousLevel : 1;			  // 0x22a(0x01)
	char pad_22A_7 : 1;						  // 0x22a(0x01)
	char pad_22B[0x1];						  // 0x22b(0x01)
	int32_t StartTime;						  // 0x22c(0x04)
	struct ULocalMessage* EngineMessageClass; // 0x230(0x08)
	char pad_238[0x8];						  // 0x238(0x08)
	struct FString SavedNetworkAddress;		  // 0x240(0x10)
	char UniqueId[0x28];					  // 0x250(0x28)
	char pad_278[0x8];						  // 0x278(0x08)
	struct APawn* PawnPrivate;				  // 0x280(0x08)
	char pad_288[0x78];						  // 0x288(0x78)
	struct FString PlayerNamePrivate;		  // 0x300(0x10)
	char pad_310[0x10];						  // 0x310(0x10)

	void ReceiveOverrideWith(struct APlayerState* OldPlayerState);	 // Function Engine.PlayerState.ReceiveOverrideWith // (Event|Protected|BlueprintEvent) // @ game+0x1a5c6b0
	void ReceiveCopyProperties(struct APlayerState* NewPlayerState); // Function Engine.PlayerState.ReceiveCopyProperties // (Event|Protected|BlueprintEvent) // @ game+0x1a5c6b0
	void OnRep_UniqueId();											 // Function Engine.PlayerState.OnRep_UniqueId // (Native|Public) // @ game+0x8f39e0
	void OnRep_Score();												 // Function Engine.PlayerState.OnRep_Score // (Native|Public) // @ game+0x164ce00
	void OnRep_PlayerName();										 // Function Engine.PlayerState.OnRep_PlayerName // (Native|Public) // @ game+0x16a2a70
	void OnRep_PlayerId();											 // Function Engine.PlayerState.OnRep_PlayerId // (Native|Public) // @ game+0x165c3b0
	void OnRep_bIsSpectator();										 // Function Engine.PlayerState.OnRep_bIsSpectator // (Native|Public) // @ game+0x16bd340
	void OnRep_bIsInactive();										 // Function Engine.PlayerState.OnRep_bIsInactive // (Native|Public) // @ game+0x16f3390
	bool IsOnlyASpectator();										 // Function Engine.PlayerState.IsOnlyASpectator // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x37bafa0
	struct FString GetPlayerName();									 // Function Engine.PlayerState.GetPlayerName // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x37badb0
};
