#pragma once

#include "Fwd.h"
#include "Values.h"
#include "APawn.h"

// Class Engine.Character
// Size: 0x4c0 (Inherited: 0x280)
struct ACharacter : APawn
{
	struct USkeletalMeshComponent* Mesh;										 // 0x280(0x08)
	struct UCharacterMovementComponent* CharacterMovement;						 // 0x288(0x08)
	struct UCapsuleComponent* CapsuleComponent;									 // 0x290(0x08)
	char BasedMovement[0x30];													 // 0x298(0x30)
	char ReplicatedBasedMovement[0x30];											 // 0x2c8(0x30)
	float AnimRootMotionTranslationScale;										 // 0x2f8(0x04)
	struct FVector BaseTranslationOffset;										 // 0x2fc(0x0c)
	char pad_308[0x8];															 // 0x308(0x08)
	char BaseRotationOffset[0x10];												 // 0x310(0x10)
	float ReplicatedServerLastTransformUpdateTimeStamp;							 // 0x320(0x04)
	float ReplayLastTransformUpdateTimeStamp;									 // 0x324(0x04)
	char ReplicatedMovementMode;												 // 0x328(0x01)
	bool bInBaseReplication;													 // 0x329(0x01)
	char pad_32A[0x2];															 // 0x32a(0x02)
	float CrouchedEyeHeight;													 // 0x32c(0x04)
	char bIsCrouched : 1;														 // 0x330(0x01)
	char bProxyIsJumpForceApplied : 1;											 // 0x330(0x01)
	char bPressedJump : 1;														 // 0x330(0x01)
	char bClientUpdating : 1;													 // 0x330(0x01)
	char bClientWasFalling : 1;													 // 0x330(0x01)
	char bClientResimulateRootMotion : 1;										 // 0x330(0x01)
	char bClientResimulateRootMotionSources : 1;								 // 0x330(0x01)
	char bSimGravityDisabled : 1;												 // 0x330(0x01)
	char bClientCheckEncroachmentOnNetUpdate : 1;								 // 0x331(0x01)
	char bServerMoveIgnoreRootMotion : 1;										 // 0x331(0x01)
	char bWasJumping : 1;														 // 0x331(0x01)
	char pad_331_3 : 5;															 // 0x331(0x01)
	char pad_332[0x2];															 // 0x332(0x02)
	float JumpKeyHoldTime;														 // 0x334(0x04)
	float JumpForceTimeRemaining;												 // 0x338(0x04)
	float ProxyJumpForceStartedTime;											 // 0x33c(0x04)
	float JumpMaxHoldTime;														 // 0x340(0x04)
	int32_t JumpMaxCount;														 // 0x344(0x04)
	int32_t JumpCurrentCount;													 // 0x348(0x04)
	int32_t JumpCurrentCountPreJump;											 // 0x34c(0x04)
	char pad_350[0x8];															 // 0x350(0x08)
	struct FMulticastInlineDelegate OnReachedJumpApex;							 // 0x358(0x10)
	char pad_368[0x10];															 // 0x368(0x10)
	struct FMulticastInlineDelegate MovementModeChangedDelegate;				 // 0x378(0x10)
	struct FMulticastInlineDelegate OnCharacterMovementUpdated;					 // 0x388(0x10)
	char SavedRootMotion[0x38];													 // 0x398(0x38)
	char ClientRootMotionParams[0x40];											 // 0x3d0(0x40)
	struct TArray<struct FSimulatedRootMotionReplicatedMove> RootMotionRepMoves; // 0x410(0x10)
	char RepRootMotion[0x98];													 // 0x420(0x98)
	char pad_4B8[0x8];															 // 0x4b8(0x08)

	void UnCrouch(bool bClientSimulation);																																																																																										 // Function Engine.Character.UnCrouch // (Native|Public|BlueprintCallable) // @ game+0x36c86d0
	void StopJumping();																																																																																															 // Function Engine.Character.StopJumping // (Native|Public|BlueprintCallable) // @ game+0x1696150
	void StopAnimMontage(struct UAnimMontage* AnimMontage);																																																																																						 // Function Engine.Character.StopAnimMontage // (Native|Public|BlueprintCallable) // @ game+0x36c8640
	void ServerMovePacked(struct FCharacterServerMovePackedBits PackedBits);																																																																																	 // Function Engine.Character.ServerMovePacked // (Net|Native|Event|Public|NetServer|NetValidate) // @ game+0x36c8270
	void ServerMoveOld(float OldTimeStamp, struct FVector_NetQuantize10 OldAccel, char OldMoveFlags);																																																																											 // Function Engine.Character.ServerMoveOld // (Net|Native|Event|Public|NetServer|NetValidate) // @ game+0x36c8100
	void ServerMoveNoBase(float Timestamp, struct FVector_NetQuantize10 InAccel, struct FVector_NetQuantize100 ClientLoc, char CompressedMoveFlags, char ClientRoll, uint32_t View, char ClientMovementMode);																																																	 // Function Engine.Character.ServerMoveNoBase // (Net|Native|Event|Public|NetServer|NetValidate) // @ game+0x36c7e60
	void ServerMoveDualNoBase(float TimeStamp0, struct FVector_NetQuantize10 InAccel0, char PendingFlags, uint32_t View0, float Timestamp, struct FVector_NetQuantize10 InAccel, struct FVector_NetQuantize100 ClientLoc, char NewFlags, char ClientRoll, uint32_t View, char ClientMovementMode);																												 // Function Engine.Character.ServerMoveDualNoBase // (Net|Native|Event|Public|NetServer|NetValidate) // @ game+0x36c7a60
	void ServerMoveDualHybridRootMotion(float TimeStamp0, struct FVector_NetQuantize10 InAccel0, char PendingFlags, uint32_t View0, float Timestamp, struct FVector_NetQuantize10 InAccel, struct FVector_NetQuantize100 ClientLoc, char NewFlags, char ClientRoll, uint32_t View, struct UPrimitiveComponent* ClientMovementBase, struct FName ClientBaseBoneName, char ClientMovementMode);					 // Function Engine.Character.ServerMoveDualHybridRootMotion // (Net|Native|Event|Public|NetServer|NetValidate) // @ game+0x36c75d0
	void ServerMoveDual(float TimeStamp0, struct FVector_NetQuantize10 InAccel0, char PendingFlags, uint32_t View0, float Timestamp, struct FVector_NetQuantize10 InAccel, struct FVector_NetQuantize100 ClientLoc, char NewFlags, char ClientRoll, uint32_t View, struct UPrimitiveComponent* ClientMovementBase, struct FName ClientBaseBoneName, char ClientMovementMode);									 // Function Engine.Character.ServerMoveDual // (Net|Native|Event|Public|NetServer|NetValidate) // @ game+0x36c7140
	void ServerMove(float Timestamp, struct FVector_NetQuantize10 InAccel, struct FVector_NetQuantize100 ClientLoc, char CompressedMoveFlags, char ClientRoll, uint32_t View, struct UPrimitiveComponent* ClientMovementBase, struct FName ClientBaseBoneName, char ClientMovementMode);																														 // Function Engine.Character.ServerMove // (Net|Native|Event|Public|NetServer|NetValidate) // @ game+0x36c6e00
	void RootMotionDebugClientPrintOnScreen(struct FString inString);																																																																																			 // Function Engine.Character.RootMotionDebugClientPrintOnScreen // (Net|NetReliableNative|Event|Public|NetClient) // @ game+0x36c6d60
	float PlayAnimMontage(struct UAnimMontage* AnimMontage, float InPlayRate, struct FName StartSectionName);																																																																									 // Function Engine.Character.PlayAnimMontage // (Native|Public|BlueprintCallable) // @ game+0x36c6c40
	void OnWalkingOffLedge(struct FVector& PreviousFloorImpactNormal, struct FVector& PreviousFloorContactNormal, struct FVector& PreviousLocation, float TimeDelta);																																																											 // Function Engine.Character.OnWalkingOffLedge // (Native|Event|Public|HasOutParms|HasDefaults|BlueprintEvent) // @ game+0x36c6aa0
	void OnRep_RootMotion();																																																																																													 // Function Engine.Character.OnRep_RootMotion // (Final|Native|Public) // @ game+0x36c6a80
	void OnRep_ReplicatedBasedMovement();																																																																																										 // Function Engine.Character.OnRep_ReplicatedBasedMovement // (Native|Public) // @ game+0x16a2e50
	void OnRep_ReplayLastTransformUpdateTimeStamp();																																																																																							 // Function Engine.Character.OnRep_ReplayLastTransformUpdateTimeStamp // (Final|Native|Public) // @ game+0x36c6a60
	void OnRep_IsCrouched();																																																																																													 // Function Engine.Character.OnRep_IsCrouched // (Native|Public) // @ game+0x16a3ab0
	void OnLaunched(struct FVector LaunchVelocity, bool bXYOverride, bool bZOverride);																																																																															 // Function Engine.Character.OnLaunched // (Event|Public|HasDefaults|BlueprintEvent) // @ game+0x1a5c6b0
	void OnLanded(struct FHitResult& Hit);																																																																																										 // Function Engine.Character.OnLanded // (Event|Public|HasOutParms|BlueprintEvent) // @ game+0x1a5c6b0
	void OnJumped();																																																																																															 // Function Engine.Character.OnJumped // (Native|Event|Public|BlueprintEvent) // @ game+0x36c6a40
	void LaunchCharacter(struct FVector LaunchVelocity, bool bXYOverride, bool bZOverride);																																																																														 // Function Engine.Character.LaunchCharacter // (Native|Public|HasDefaults|BlueprintCallable) // @ game+0x36c6910
	void K2_UpdateCustomMovement(float DeltaTime);																																																																																								 // Function Engine.Character.K2_UpdateCustomMovement // (Event|Public|BlueprintEvent) // @ game+0x1a5c6b0
	void K2_OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust);																																																																																 // Function Engine.Character.K2_OnStartCrouch // (Event|Public|BlueprintEvent) // @ game+0x1a5c6b0
	void K2_OnMovementModeChanged(enum class EMovementMode PrevMovementMode, enum class EMovementMode NewMovementMode, char PrevCustomMode, char NewCustomMode);																																																												 // Function Engine.Character.K2_OnMovementModeChanged // (Event|Public|BlueprintEvent) // @ game+0x1a5c6b0
	void K2_OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust);																																																																																	 // Function Engine.Character.K2_OnEndCrouch // (Event|Public|BlueprintEvent) // @ game+0x1a5c6b0
	void Jump();																																																																																																 // Function Engine.Character.Jump // (Native|Public|BlueprintCallable) // @ game+0x36c68f0
	bool IsPlayingRootMotion();																																																																																													 // Function Engine.Character.IsPlayingRootMotion // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x36c68c0
	bool IsPlayingNetworkedRootMotionMontage();																																																																																									 // Function Engine.Character.IsPlayingNetworkedRootMotionMontage // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x36c6890
	bool IsJumpProvidingForce();																																																																																												 // Function Engine.Character.IsJumpProvidingForce // (Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x36c6860
	bool HasAnyRootMotion();																																																																																													 // Function Engine.Character.HasAnyRootMotion // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x36c6830
	struct UAnimMontage* GetCurrentMontage();																																																																																									 // Function Engine.Character.GetCurrentMontage // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x36c61a0
	struct FVector GetBaseTranslationOffset();																																																																																									 // Function Engine.Character.GetBaseTranslationOffset // (Final|Native|Public|HasDefaults|BlueprintCallable|BlueprintPure|Const) // @ game+0x36c6170
	struct FRotator GetBaseRotationOffsetRotator();																																																																																								 // Function Engine.Character.GetBaseRotationOffsetRotator // (Final|Native|Public|HasDefaults|BlueprintCallable|BlueprintPure|Const) // @ game+0x36c6120
	float GetAnimRootMotionTranslationScale();																																																																																									 // Function Engine.Character.GetAnimRootMotionTranslationScale // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x36c60f0
	void Crouch(bool bClientSimulation);																																																																																										 // Function Engine.Character.Crouch // (Native|Public|BlueprintCallable) // @ game+0x36c6060
	void ClientVeryShortAdjustPosition(float Timestamp, struct FVector NewLoc, struct UPrimitiveComponent* NewBase, struct FName NewBaseBoneName, bool bHasBase, bool bBaseRelativePosition, char ServerMovementMode);																																															 // Function Engine.Character.ClientVeryShortAdjustPosition // (Net|Native|Event|Public|HasDefaults|NetClient) // @ game+0x36c5ce0
	void ClientMoveResponsePacked(struct FCharacterMoveResponsePackedBits PackedBits);																																																																															 // Function Engine.Character.ClientMoveResponsePacked // (Net|Native|Event|Public|NetClient|NetValidate) // @ game+0x36c5bf0
	void ClientCheatWalk();																																																																																														 // Function Engine.Character.ClientCheatWalk // (Net|NetReliableNative|Event|Public|NetClient) // @ game+0x36c5bd0
	void ClientCheatGhost();																																																																																													 // Function Engine.Character.ClientCheatGhost // (Net|NetReliableNative|Event|Public|NetClient) // @ game+0x36c5bb0
	void ClientCheatFly();																																																																																														 // Function Engine.Character.ClientCheatFly // (Net|NetReliableNative|Event|Public|NetClient) // @ game+0x36c5b90
	void ClientAdjustRootMotionSourcePosition(float Timestamp, struct FRootMotionSourceGroup ServerRootMotion, bool bHasAnimRootMotion, float ServerMontageTrackPosition, struct FVector ServerLoc, struct FVector_NetQuantizeNormal ServerRotation, float ServerVelZ, struct UPrimitiveComponent* ServerBase, struct FName ServerBoneName, bool bHasBase, bool bBaseRelativePosition, char ServerMovementMode); // Function Engine.Character.ClientAdjustRootMotionSourcePosition // (Net|Native|Event|Public|HasDefaults|NetClient) // @ game+0x36c56b0
	void ClientAdjustRootMotionPosition(float Timestamp, float ServerMontageTrackPosition, struct FVector ServerLoc, struct FVector_NetQuantizeNormal ServerRotation, float ServerVelZ, struct UPrimitiveComponent* ServerBase, struct FName ServerBoneName, bool bHasBase, bool bBaseRelativePosition, char ServerMovementMode);																				 // Function Engine.Character.ClientAdjustRootMotionPosition // (Net|Native|Event|Public|HasDefaults|NetClient) // @ game+0x36c5380
	void ClientAdjustPosition(float Timestamp, struct FVector NewLoc, struct FVector NewVel, struct UPrimitiveComponent* NewBase, struct FName NewBaseBoneName, bool bHasBase, bool bBaseRelativePosition, char ServerMovementMode);																																											 // Function Engine.Character.ClientAdjustPosition // (Net|Native|Event|Public|HasDefaults|NetClient) // @ game+0x36c50f0
	void ClientAckGoodMove(float Timestamp);																																																																																									 // Function Engine.Character.ClientAckGoodMove // (Net|Native|Event|Public|NetClient) // @ game+0x36c5070
	bool CanJumpInternal();																																																																																														 // Function Engine.Character.CanJumpInternal // (Native|Event|Protected|BlueprintEvent|Const) // @ game+0x36c5040
	bool CanJump();																																																																																																 // Function Engine.Character.CanJump // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x36c5010
	bool CanCrouch();																																																																																															 // Function Engine.Character.CanCrouch // (Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x36c4fe0
	void CacheInitialMeshOffset(struct FVector MeshRelativeLocation, struct FRotator MeshRelativeRotation);																																																																										 // Function Engine.Character.CacheInitialMeshOffset // (Native|Public|HasDefaults|BlueprintCallable) // @ game+0x36c4ef0

	static struct UClass* StaticClass();
};
