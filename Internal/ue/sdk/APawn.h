#pragma once

#include "Fwd.h"
#include "Values.h"
#include "AActor.h"

// Class Engine.Pawn
// Size: 0x280 (Inherited: 0x220)
struct APawn : AActor
{
	char pad_220[0x8];						 // 0x220(0x08)
	char bUseControllerRotationPitch : 1;	 // 0x228(0x01)
	char bUseControllerRotationYaw : 1;		 // 0x228(0x01)
	char bUseControllerRotationRoll : 1;	 // 0x228(0x01)
	char bCanAffectNavigationGeneration : 1; // 0x228(0x01)
	char pad_228_4 : 4;						 // 0x228(0x01)
	char pad_229[0x3];						 // 0x229(0x03)
	float BaseEyeHeight;					 // 0x22c(0x04)
	char AutoPossessPlayer[0x01];			 // 0x230(0x01)
	char AutoPossessAI[0x01];				 // 0x231(0x01)
	char RemoteViewPitch[0x01];				 // 0x232(0x01)
	char pad_233[0x5];						 // 0x233(0x05)
	struct AController* AIControllerClass;	 // 0x238(0x08)
	struct APlayerState* PlayerState;		 // 0x240(0x08)
	char pad_248[0x8];						 // 0x248(0x08)
	struct AController* LastHitBy;			 // 0x250(0x08)
	struct AController* Controller;			 // 0x258(0x08)
	char pad_260[0x4];						 // 0x260(0x04)
	struct FVector ControlInputVector;		 // 0x264(0x0c)
	struct FVector LastControlInputVector;	 // 0x270(0x0c)
	char pad_27C[0x4];						 // 0x27c(0x04)

	void SpawnDefaultController();																							  // Function Engine.Pawn.SpawnDefaultController // (Native|Public|BlueprintCallable) // @ game+0x1695cd0
	void SetCanAffectNavigationGeneration(bool bNewValue, bool bForceUpdate);												  // Function Engine.Pawn.SetCanAffectNavigationGeneration // (Final|Native|Public|BlueprintCallable) // @ game+0x37a6280
	void ReceiveUnpossessed(struct AController* OldController);																  // Function Engine.Pawn.ReceiveUnpossessed // (Event|Public|BlueprintEvent) // @ game+0x1a5c6b0
	void ReceivePossessed(struct AController* NewController);																  // Function Engine.Pawn.ReceivePossessed // (Event|Public|BlueprintEvent) // @ game+0x1a5c6b0
	void PawnMakeNoise(float Loudness, struct FVector NoiseLocation, bool bUseNoiseMakerLocation, struct AActor* NoiseMaker); // Function Engine.Pawn.PawnMakeNoise // (Final|BlueprintAuthorityOnly|Native|Public|HasDefaults|BlueprintCallable) // @ game+0x37a5720
	void OnRep_PlayerState();																								  // Function Engine.Pawn.OnRep_PlayerState // (Native|Public) // @ game+0x8f3b20
	void OnRep_Controller();																								  // Function Engine.Pawn.OnRep_Controller // (Native|Public) // @ game+0x16be520
	void LaunchPawn(struct FVector LaunchVelocity, bool bXYOverride, bool bZOverride);										  // Function Engine.Pawn.LaunchPawn // (Final|Native|Public|HasDefaults|BlueprintCallable) // @ game+0x37a54e0
	struct FVector K2_GetMovementInputVector();																				  // Function Engine.Pawn.K2_GetMovementInputVector // (Final|Native|Public|HasDefaults|BlueprintCallable|BlueprintPure|Const) // @ game+0x37a5380
	bool IsPlayerControlled();																								  // Function Engine.Pawn.IsPlayerControlled // (Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x36d0ba0
	bool IsPawnControlled();																								  // Function Engine.Pawn.IsPawnControlled // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x37a54b0
	bool IsMoveInputIgnored();																								  // Function Engine.Pawn.IsMoveInputIgnored // (Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x37a5450
	bool IsLocallyControlled();																								  // Function Engine.Pawn.IsLocallyControlled // (Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x37a5420
	bool IsControlled();																									  // Function Engine.Pawn.IsControlled // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x37a53f0
	bool IsBotControlled();																									  // Function Engine.Pawn.IsBotControlled // (Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x36f3aa0
	struct FVector GetPendingMovementInputVector();																			  // Function Engine.Pawn.GetPendingMovementInputVector // (Final|Native|Public|HasDefaults|BlueprintCallable|BlueprintPure|Const) // @ game+0x37a5380
	struct FVector GetNavAgentLocation();																					  // Function Engine.Pawn.GetNavAgentLocation // (Native|Public|HasDefaults|BlueprintCallable|BlueprintPure|Const) // @ game+0x37a52c0
	struct UPawnMovementComponent* GetMovementComponent();																	  // Function Engine.Pawn.GetMovementComponent // (Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x36d96c0
	struct AActor* GetMovementBaseActor(struct APawn* Pawn);																  // Function Engine.Pawn.GetMovementBaseActor // (Final|Native|Static|Public|BlueprintCallable|BlueprintPure) // @ game+0x37a5230
	struct FVector GetLastMovementInputVector();																			  // Function Engine.Pawn.GetLastMovementInputVector // (Final|Native|Public|HasDefaults|BlueprintCallable|BlueprintPure|Const) // @ game+0x37a51f0
	struct FRotator GetControlRotation();																					  // Function Engine.Pawn.GetControlRotation // (Final|Native|Public|HasDefaults|BlueprintCallable|BlueprintPure|Const) // @ game+0x37a50c0
	struct AController* GetController();																					  // Function Engine.Pawn.GetController // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x37a5100
	struct FRotator GetBaseAimRotation();																					  // Function Engine.Pawn.GetBaseAimRotation // (Native|Public|HasDefaults|BlueprintCallable|BlueprintPure|Const) // @ game+0x37a4ee0
	void DetachFromControllerPendingDestroy();																				  // Function Engine.Pawn.DetachFromControllerPendingDestroy // (Native|Public|BlueprintCallable) // @ game+0x16964e0
	struct FVector ConsumeMovementInputVector();																			  // Function Engine.Pawn.ConsumeMovementInputVector // (Native|Public|HasDefaults|BlueprintCallable) // @ game+0x37a4ea0
	void AddMovementInput(struct FVector WorldDirection, float ScaleValue, bool bForce);									  // Function Engine.Pawn.AddMovementInput // (Native|Public|HasDefaults|BlueprintCallable) // @ game+0x37a4980
	void AddControllerYawInput(float Val);																					  // Function Engine.Pawn.AddControllerYawInput // (Native|Public|BlueprintCallable) // @ game+0x37a4810
	void AddControllerRollInput(float Val);																					  // Function Engine.Pawn.AddControllerRollInput // (Native|Public|BlueprintCallable) // @ game+0x37a4780
	void AddControllerPitchInput(float Val);																				  // Function Engine.Pawn.AddControllerPitchInput // (Native|Public|BlueprintCallable) // @ game+0x37a46f0

	static struct UClass* StaticClass();
};
