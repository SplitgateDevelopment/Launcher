#pragma once

#include "Fwd.h"
#include "Values.h"
#include "AActor.h"

// Class Engine.Controller
// Size: 0x298 (Inherited: 0x220)
struct AController : AActor
{
	char pad_220[0x8];									   // 0x220(0x08)
	struct APlayerState* PlayerState;					   // 0x228(0x08)
	char pad_230[0x8];									   // 0x230(0x08)
	struct FMulticastInlineDelegate OnInstigatedAnyDamage; // 0x238(0x10)
	struct FName StateName;								   // 0x248(0x08)
	struct APawn* Pawn;									   // 0x250(0x08)
	char pad_258[0x8];									   // 0x258(0x08)
	struct ACharacter* Character;						   // 0x260(0x08)
	struct USceneComponent* TransformComponent;			   // 0x268(0x08)
	char pad_270[0x18];									   // 0x270(0x18)
	struct FRotator ControlRotation;					   // 0x288(0x0c)
	char bAttachToPawn : 1;								   // 0x294(0x01)
	char pad_294_1 : 7;									   // 0x294(0x01)
	char pad_295[0x3];									   // 0x295(0x03)

	void UnPossess();																														 // Function Engine.Controller.UnPossess // (Final|Native|Public|BlueprintCallable) // @ game+0x36d1160
	void StopMovement();																													 // Function Engine.Controller.StopMovement // (Native|Public|BlueprintCallable) // @ game+0x1695cb0
	void SetInitialLocationAndRotation(struct FVector& NewLocation, struct FRotator& NewRotation);											 // Function Engine.Controller.SetInitialLocationAndRotation // (Native|Public|HasOutParms|HasDefaults|BlueprintCallable) // @ game+0x36d1070
	void SetIgnoreMoveInput(bool bNewMoveInput);																							 // Function Engine.Controller.SetIgnoreMoveInput // (Native|Public|BlueprintCallable) // @ game+0x36d0fe0
	void SetIgnoreLookInput(bool bNewLookInput);																							 // Function Engine.Controller.SetIgnoreLookInput // (Native|Public|BlueprintCallable) // @ game+0x36d0f50
	void SetControlRotation(struct FRotator& NewRotation);																					 // Function Engine.Controller.SetControlRotation // (Native|Public|HasOutParms|HasDefaults|BlueprintCallable) // @ game+0x36d0ec0
	void ResetIgnoreMoveInput();																											 // Function Engine.Controller.ResetIgnoreMoveInput // (Native|Public|BlueprintCallable) // @ game+0x36d0ea0
	void ResetIgnoreLookInput();																											 // Function Engine.Controller.ResetIgnoreLookInput // (Native|Public|BlueprintCallable) // @ game+0x169a850
	void ResetIgnoreInputFlags();																											 // Function Engine.Controller.ResetIgnoreInputFlags // (Native|Public|BlueprintCallable) // @ game+0x1661650
	void ReceiveUnPossess(struct APawn* UnpossessedPawn);																					 // Function Engine.Controller.ReceiveUnPossess // (Event|Protected|BlueprintEvent) // @ game+0x1a5c6b0
	void ReceivePossess(struct APawn* PossessedPawn);																						 // Function Engine.Controller.ReceivePossess // (Event|Protected|BlueprintEvent) // @ game+0x1a5c6b0
	void ReceiveInstigatedAnyDamage(float Damage, struct UDamageType* DamageType, struct AActor* DamagedActor, struct AActor* DamageCauser); // Function Engine.Controller.ReceiveInstigatedAnyDamage // (BlueprintAuthorityOnly|Event|Protected|BlueprintEvent) // @ game+0x1a5c6b0
	void Possess(struct APawn* InPawn);																										 // Function Engine.Controller.Possess // (Final|BlueprintAuthorityOnly|Native|Public|BlueprintCallable) // @ game+0x36d0e10
	void OnRep_PlayerState();																												 // Function Engine.Controller.OnRep_PlayerState // (Native|Public) // @ game+0x16afd20
	void OnRep_Pawn();																														 // Function Engine.Controller.OnRep_Pawn // (Native|Public) // @ game+0x1674610
	bool LineOfSightTo(struct AActor* Other, struct FVector ViewPoint, bool bAlternateChecks);												 // Function Engine.Controller.LineOfSightTo // (Native|Public|HasDefaults|BlueprintCallable|BlueprintPure|Const) // @ game+0x36d0ce0
	struct APawn* K2_GetPawn();																												 // Function Engine.Controller.K2_GetPawn // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x36d0cb0
	bool IsPlayerController();																												 // Function Engine.Controller.IsPlayerController // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x36d0c80
	bool IsMoveInputIgnored();																												 // Function Engine.Controller.IsMoveInputIgnored // (Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x36d0c50
	bool IsLookInputIgnored();																												 // Function Engine.Controller.IsLookInputIgnored // (Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x36d0c20
	bool IsLocalPlayerController();																											 // Function Engine.Controller.IsLocalPlayerController // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x36d0bd0
	bool IsLocalController();																												 // Function Engine.Controller.IsLocalController // (Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x36d0ba0
	struct AActor* GetViewTarget();																											 // Function Engine.Controller.GetViewTarget // (Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x36d0b70
	struct FRotator GetDesiredRotation();																									 // Function Engine.Controller.GetDesiredRotation // (Native|Public|HasDefaults|BlueprintCallable|BlueprintPure|Const) // @ game+0x36d0970
	struct FRotator GetControlRotation();																									 // Function Engine.Controller.GetControlRotation // (Native|Public|HasDefaults|BlueprintCallable|BlueprintPure|Const) // @ game+0x36d0930
	void ClientSetRotation(struct FRotator NewRotation, bool bResetCamera);																	 // Function Engine.Controller.ClientSetRotation // (Net|NetReliableNative|Event|Public|HasDefaults|NetClient|NetValidate) // @ game+0x36d0800
	void ClientSetLocation(struct FVector NewLocation, struct FRotator NewRotation);														 // Function Engine.Controller.ClientSetLocation // (Net|NetReliableNative|Event|Public|HasDefaults|NetClient|NetValidate) // @ game+0x36d06c0
	struct APlayerController* CastToPlayerController();																						 // Function Engine.Controller.CastToPlayerController // (Final|Native|Public|BlueprintCallable) // @ game+0x36d0690
};
