#pragma once

#include "Fwd.h"
#include "Values.h"
#include "UObject.h"

// Class Engine.Actor
// Size: 0x220 (Inherited: 0x28)
struct AActor : UObject
{
	char PrimaryActorTick[0x30];									   // 0x28(0x30)
	char bNetTemporary : 1;											   // 0x58(0x01)
	char bNetStartup : 1;											   // 0x58(0x01)
	char bOnlyRelevantToOwner : 1;									   // 0x58(0x01)
	char bAlwaysRelevant : 1;										   // 0x58(0x01)
	char bReplicateMovement : 1;									   // 0x58(0x01)
	char bHidden : 1;												   // 0x58(0x01)
	char bTearOff : 1;												   // 0x58(0x01)
	char bForceNetAddressable : 1;									   // 0x58(0x01)
	char bExchangedRoles : 1;										   // 0x59(0x01)
	char bNetLoadOnClient : 1;										   // 0x59(0x01)
	char bNetUseOwnerRelevancy : 1;									   // 0x59(0x01)
	char bRelevantForNetworkReplays : 1;							   // 0x59(0x01)
	char bRelevantForLevelBounds : 1;								   // 0x59(0x01)
	char bReplayRewindable : 1;										   // 0x59(0x01)
	char bAllowTickBeforeBeginPlay : 1;								   // 0x59(0x01)
	char bAutoDestroyWhenFinished : 1;								   // 0x59(0x01)
	char bCanBeDamaged : 1;											   // 0x5a(0x01)
	char bBlockInput : 1;											   // 0x5a(0x01)
	char bCollideWhenPlacing : 1;									   // 0x5a(0x01)
	char bFindCameraComponentWhenViewTarget : 1;					   // 0x5a(0x01)
	char bGenerateOverlapEventsDuringLevelStreaming : 1;			   // 0x5a(0x01)
	char bIgnoresOriginShifting : 1;								   // 0x5a(0x01)
	char bEnableAutoLODGeneration : 1;								   // 0x5a(0x01)
	char bIsEditorOnlyActor : 1;									   // 0x5a(0x01)
	char bActorSeamlessTraveled : 1;								   // 0x5b(0x01)
	char bReplicates : 1;											   // 0x5b(0x01)
	char bCanBeInCluster : 1;										   // 0x5b(0x01)
	char bAllowReceiveTickEventOnDedicatedServer : 1;				   // 0x5b(0x01)
	char pad_5B_4 : 4;												   // 0x5b(0x01)
	char pad_5C_0 : 3;												   // 0x5c(0x01)
	char bActorEnableCollision : 1;									   // 0x5c(0x01)
	char bActorIsBeingDestroyed : 1;								   // 0x5c(0x01)
	char pad_5C_5 : 3;												   // 0x5c(0x01)
	char UpdateOverlapsMethodDuringLevelStreaming[0x01];			   // 0x5d(0x01)
	char DefaultUpdateOverlapsMethodDuringLevelStreaming[0x01];		   // 0x5e(0x01)
	char RemoteRole[0x01];											   // 0x5f(0x01)
	char ReplicatedMovement[0x34];									   // 0x60(0x34)
	float InitialLifeSpan;											   // 0x94(0x04)
	float CustomTimeDilation;										   // 0x98(0x04)
	char pad_9C[0x4];												   // 0x9c(0x04)
	char AttachmentReplication[0x40];								   // 0xa0(0x40)
	struct AActor* Owner;											   // 0xe0(0x08)
	struct FName NetDriverName;										   // 0xe8(0x08)
	char Role[0x01];												   // 0xf0(0x01)
	char NetDormancy[0x01];											   // 0xf1(0x01)
	char SpawnCollisionHandlingMethod[0x01];						   // 0xf2(0x01)
	char AutoReceiveInput[0x01];									   // 0xf3(0x01)
	int32_t InputPriority;											   // 0xf4(0x04)
	struct UInputComponent* InputComponent;							   // 0xf8(0x08)
	float NetCullDistanceSquared;									   // 0x100(0x04)
	int32_t NetTag;													   // 0x104(0x04)
	float NetUpdateFrequency;										   // 0x108(0x04)
	float MinNetUpdateFrequency;									   // 0x10c(0x04)
	float NetPriority;												   // 0x110(0x04)
	char pad_114[0x4];												   // 0x114(0x04)
	struct APawn* Instigator;										   // 0x118(0x08)
	struct TArray<struct AActor*> Children;							   // 0x120(0x10)
	struct USceneComponent* RootComponent;							   // 0x130(0x08)
	struct TArray<struct AMatineeActor*> ControllingMatineeActors;	   // 0x138(0x10)
	char pad_148[0x8];												   // 0x148(0x08)
	struct TArray<struct FName> Layers;								   // 0x150(0x10)
	char ParentComponent[0x08];										   // 0x160(0x08)
	char pad_168[0x8];												   // 0x168(0x08)
	struct TArray<struct FName> Tags;								   // 0x170(0x10)
	char OnTakeAnyDamage[0x01];										   // 0x180(0x01)
	char OnTakePointDamage[0x01];									   // 0x181(0x01)
	char OnTakeRadialDamage[0x01];									   // 0x182(0x01)
	char OnActorBeginOverlap[0x01];									   // 0x183(0x01)
	char OnActorEndOverlap[0x01];									   // 0x184(0x01)
	char OnBeginCursorOver[0x01];									   // 0x185(0x01)
	char OnEndCursorOver[0x01];										   // 0x186(0x01)
	char OnClicked[0x01];											   // 0x187(0x01)
	char OnReleased[0x01];											   // 0x188(0x01)
	char OnInputTouchBegin[0x01];									   // 0x189(0x01)
	char OnInputTouchEnd[0x01];										   // 0x18a(0x01)
	char OnInputTouchEnter[0x01];									   // 0x18b(0x01)
	char OnInputTouchLeave[0x01];									   // 0x18c(0x01)
	char OnActorHit[0x01];											   // 0x18d(0x01)
	char OnDestroyed[0x01];											   // 0x18e(0x01)
	char OnEndPlay[0x01];											   // 0x18f(0x01)
	char pad_190[0x60];												   // 0x190(0x60)
	struct TArray<struct UActorComponent*> InstanceComponents;		   // 0x1f0(0x10)
	struct TArray<struct UActorComponent*> BlueprintCreatedComponents; // 0x200(0x10)
	char pad_210[0x10];												   // 0x210(0x10)

	bool WasRecentlyRendered(float Tolerance);																																																																				   // Function Engine.Actor.WasRecentlyRendered // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x368d6b0
	void UserConstructionScript();																																																																							   // Function Engine.Actor.UserConstructionScript // (Event|Public|BlueprintEvent) // @ game+0x1a5c6b0
	void TearOff();																																																																											   // Function Engine.Actor.TearOff // (Native|Public|BlueprintCallable) // @ game+0x27d8180
	void SnapRootComponentTo(struct AActor* InParentActor, struct FName InSocketName);																																																										   // Function Engine.Actor.SnapRootComponentTo // (Final|Native|Public|BlueprintCallable) // @ game+0x368d5e0
	void SetTickGroup(enum class ETickingGroup NewTickGroup);																																																																   // Function Engine.Actor.SetTickGroup // (Final|Native|Public|BlueprintCallable) // @ game+0x368d4d0
	void SetTickableWhenPaused(bool bTickableWhenPaused);																																																																	   // Function Engine.Actor.SetTickableWhenPaused // (Final|Native|Public|BlueprintCallable) // @ game+0x368d550
	void SetReplicates(bool bInReplicates);																																																																					   // Function Engine.Actor.SetReplicates // (Final|BlueprintAuthorityOnly|Native|Public|BlueprintCallable) // @ game+0x368d440
	void SetReplicateMovement(bool bInReplicateMovement);																																																																	   // Function Engine.Actor.SetReplicateMovement // (Native|Public|BlueprintCallable) // @ game+0x368d3b0
	void SetOwner(struct AActor* NewOwner);																																																																					   // Function Engine.Actor.SetOwner // (Native|Public|BlueprintCallable) // @ game+0x368d320
	void SetNetDormancy(enum class ENetDormancy NewDormancy);																																																																   // Function Engine.Actor.SetNetDormancy // (Final|BlueprintAuthorityOnly|Native|Public|BlueprintCallable) // @ game+0x368d2a0
	void SetLifeSpan(float InLifespan);																																																																						   // Function Engine.Actor.SetLifeSpan // (Native|Public|BlueprintCallable) // @ game+0x368d210
	void SetAutoDestroyWhenFinished(bool bVal);																																																																				   // Function Engine.Actor.SetAutoDestroyWhenFinished // (Final|Native|Public|BlueprintCallable) // @ game+0x368d180
	void SetActorTickInterval(float TickInterval);																																																																			   // Function Engine.Actor.SetActorTickInterval // (Final|Native|Public|BlueprintCallable) // @ game+0x368d100
	void SetActorTickEnabled(bool bEnabled);																																																																				   // Function Engine.Actor.SetActorTickEnabled // (Final|Native|Public|BlueprintCallable) // @ game+0x368d070
	void SetActorScale3D(struct FVector NewScale3D);																																																																		   // Function Engine.Actor.SetActorScale3D // (Final|Native|Public|HasDefaults|BlueprintCallable) // @ game+0x368cfe0
	void SetActorRelativeScale3D(struct FVector NewRelativeScale);																																																															   // Function Engine.Actor.SetActorRelativeScale3D // (Final|Native|Public|HasDefaults|BlueprintCallable) // @ game+0x368cf50
	void SetActorHiddenInGame(bool bNewHidden);																																																																				   // Function Engine.Actor.SetActorHiddenInGame // (Native|Public|BlueprintCallable) // @ game+0x368cec0
	void SetActorEnableCollision(bool bNewActorEnableCollision);																																																															   // Function Engine.Actor.SetActorEnableCollision // (Final|Native|Public|BlueprintCallable) // @ game+0x368ce30
	void RemoveTickPrerequisiteComponent(struct UActorComponent* PrerequisiteComponent);																																																									   // Function Engine.Actor.RemoveTickPrerequisiteComponent // (Native|Public|BlueprintCallable) // @ game+0x368cda0
	void RemoveTickPrerequisiteActor(struct AActor* PrerequisiteActor);																																																														   // Function Engine.Actor.RemoveTickPrerequisiteActor // (Native|Public|BlueprintCallable) // @ game+0x368cd10
	void ReceiveTick(float DeltaSeconds);																																																																					   // Function Engine.Actor.ReceiveTick // (Event|Public|BlueprintEvent) // @ game+0x1a5c6b0
	void ReceiveRadialDamage(float DamageReceived, struct UDamageType* DamageType, struct FVector Origin, struct FHitResult& HitInfo, struct AController* InstigatedBy, struct AActor* DamageCauser);																														   // Function Engine.Actor.ReceiveRadialDamage // (BlueprintAuthorityOnly|Event|Public|HasOutParms|HasDefaults|BlueprintEvent) // @ game+0x1a5c6b0
	void ReceivePointDamage(float Damage, struct UDamageType* DamageType, struct FVector HitLocation, struct FVector HitNormal, struct UPrimitiveComponent* HitComponent, struct FName BoneName, struct FVector ShotFromDirection, struct AController* InstigatedBy, struct AActor* DamageCauser, struct FHitResult& HitInfo); // Function Engine.Actor.ReceivePointDamage // (BlueprintAuthorityOnly|Event|Public|HasOutParms|HasDefaults|BlueprintEvent) // @ game+0x1a5c6b0
	void ReceiveHit(struct UPrimitiveComponent* MyComp, struct AActor* Other, struct UPrimitiveComponent* OtherComp, bool bSelfMoved, struct FVector HitLocation, struct FVector HitNormal, struct FVector NormalImpulse, struct FHitResult& Hit);																			   // Function Engine.Actor.ReceiveHit // (Event|Public|HasOutParms|HasDefaults|BlueprintEvent) // @ game+0x1a5c6b0
	void ReceiveEndPlay(enum class EEndPlayReason EndPlayReason);																																																															   // Function Engine.Actor.ReceiveEndPlay // (Event|Protected|BlueprintEvent) // @ game+0x1a5c6b0
	void ReceiveDestroyed();																																																																								   // Function Engine.Actor.ReceiveDestroyed // (Event|Public|BlueprintEvent) // @ game+0x1a5c6b0
	void ReceiveBeginPlay();																																																																								   // Function Engine.Actor.ReceiveBeginPlay // (Event|Protected|BlueprintEvent) // @ game+0x1a5c6b0
	void ReceiveAnyDamage(float Damage, struct UDamageType* DamageType, struct AController* InstigatedBy, struct AActor* DamageCauser);																																														   // Function Engine.Actor.ReceiveAnyDamage // (BlueprintAuthorityOnly|Event|Public|BlueprintEvent) // @ game+0x1a5c6b0
	void ReceiveActorOnReleased(struct FKey ButtonReleased);																																																																   // Function Engine.Actor.ReceiveActorOnReleased // (Event|Public|BlueprintEvent) // @ game+0x1a5c6b0
	void ReceiveActorOnInputTouchLeave(enum class ETouchIndex FingerIndex);																																																													   // Function Engine.Actor.ReceiveActorOnInputTouchLeave // (Event|Public|BlueprintEvent) // @ game+0x1a5c6b0
	void ReceiveActorOnInputTouchEnter(enum class ETouchIndex FingerIndex);																																																													   // Function Engine.Actor.ReceiveActorOnInputTouchEnter // (Event|Public|BlueprintEvent) // @ game+0x1a5c6b0
	void ReceiveActorOnInputTouchEnd(enum class ETouchIndex FingerIndex);																																																													   // Function Engine.Actor.ReceiveActorOnInputTouchEnd // (Event|Public|BlueprintEvent) // @ game+0x1a5c6b0
	void ReceiveActorOnInputTouchBegin(enum class ETouchIndex FingerIndex);																																																													   // Function Engine.Actor.ReceiveActorOnInputTouchBegin // (Event|Public|BlueprintEvent) // @ game+0x1a5c6b0
	void ReceiveActorOnClicked(struct FKey ButtonPressed);																																																																	   // Function Engine.Actor.ReceiveActorOnClicked // (Event|Public|BlueprintEvent) // @ game+0x1a5c6b0
	void ReceiveActorEndOverlap(struct AActor* OtherActor);																																																																	   // Function Engine.Actor.ReceiveActorEndOverlap // (Event|Public|BlueprintEvent) // @ game+0x1a5c6b0
	void ReceiveActorEndCursorOver();																																																																						   // Function Engine.Actor.ReceiveActorEndCursorOver // (Event|Public|BlueprintEvent) // @ game+0x1a5c6b0
	void ReceiveActorBeginOverlap(struct AActor* OtherActor);																																																																   // Function Engine.Actor.ReceiveActorBeginOverlap // (Event|Public|BlueprintEvent) // @ game+0x1a5c6b0
	void ReceiveActorBeginCursorOver();																																																																						   // Function Engine.Actor.ReceiveActorBeginCursorOver // (Event|Public|BlueprintEvent) // @ game+0x1a5c6b0
	void PrestreamTextures(float Seconds, bool bEnableStreaming, int32_t CinematicTextureGroups);																																																							   // Function Engine.Actor.PrestreamTextures // (Native|Public|BlueprintCallable) // @ game+0x368cbf0
	void OnRep_ReplicateMovement();																																																																							   // Function Engine.Actor.OnRep_ReplicateMovement // (Native|Public) // @ game+0x102e110
	void OnRep_ReplicatedMovement();																																																																						   // Function Engine.Actor.OnRep_ReplicatedMovement // (Native|Public) // @ game+0x368cbd0
	void OnRep_Owner();																																																																										   // Function Engine.Actor.OnRep_Owner // (Native|Protected) // @ game+0x102df70
	void OnRep_Instigator();																																																																								   // Function Engine.Actor.OnRep_Instigator // (Native|Public) // @ game+0x368cbb0
	void OnRep_AttachmentReplication();																																																																						   // Function Engine.Actor.OnRep_AttachmentReplication // (Native|Public) // @ game+0x1034d00
	void MakeNoise(float Loudness, struct APawn* NoiseInstigator, struct FVector NoiseLocation, float MaxRange, struct FName Tag);																																															   // Function Engine.Actor.MakeNoise // (Final|BlueprintAuthorityOnly|Native|Public|HasDefaults|BlueprintCallable) // @ game+0x368ca10
	struct UMaterialInstanceDynamic* MakeMIDForMaterial(struct UMaterialInterface* Parent);																																																									   // Function Engine.Actor.MakeMIDForMaterial // (Final|Native|Public|BlueprintCallable) // @ game+0x368c970
	bool K2_TeleportTo(struct FVector DestLocation, struct FRotator DestRotation);																																																											   // Function Engine.Actor.K2_TeleportTo // (Final|Native|Public|HasDefaults|BlueprintCallable) // @ game+0x368c880
	bool K2_SetActorTransform(struct FTransform& NewTransform, bool bSweep, struct FHitResult& SweepHitResult, bool bTeleport);																																																   // Function Engine.Actor.K2_SetActorTransform // (Final|Native|Public|HasOutParms|HasDefaults|BlueprintCallable) // @ game+0x368c670
	bool K2_SetActorRotation(struct FRotator NewRotation, bool bTeleportPhysics);																																																											   // Function Engine.Actor.K2_SetActorRotation // (Final|Native|Public|HasDefaults|BlueprintCallable) // @ game+0x368c580
	void K2_SetActorRelativeTransform(struct FTransform& NewRelativeTransform, bool bSweep, struct FHitResult& SweepHitResult, bool bTeleport);																																												   // Function Engine.Actor.K2_SetActorRelativeTransform // (Final|Native|Public|HasOutParms|HasDefaults|BlueprintCallable) // @ game+0x368c370
	void K2_SetActorRelativeRotation(struct FRotator NewRelativeRotation, bool bSweep, struct FHitResult& SweepHitResult, bool bTeleport);																																													   // Function Engine.Actor.K2_SetActorRelativeRotation // (Final|Native|Public|HasOutParms|HasDefaults|BlueprintCallable) // @ game+0x368c1c0
	void K2_SetActorRelativeLocation(struct FVector NewRelativeLocation, bool bSweep, struct FHitResult& SweepHitResult, bool bTeleport);																																													   // Function Engine.Actor.K2_SetActorRelativeLocation // (Final|Native|Public|HasOutParms|HasDefaults|BlueprintCallable) // @ game+0x368c010
	bool K2_SetActorLocationAndRotation(struct FVector NewLocation, struct FRotator NewRotation, bool bSweep, struct FHitResult& SweepHitResult, bool bTeleport);																																							   // Function Engine.Actor.K2_SetActorLocationAndRotation // (Final|Native|Public|HasOutParms|HasDefaults|BlueprintCallable) // @ game+0x368be00
	bool K2_SetActorLocation(struct FVector NewLocation, bool bSweep, struct FHitResult& SweepHitResult, bool bTeleport);																																																	   // Function Engine.Actor.K2_SetActorLocation // (Final|Native|Public|HasOutParms|HasDefaults|BlueprintCallable) // @ game+0x368bc40
	void K2_OnReset();																																																																										   // Function Engine.Actor.K2_OnReset // (Event|Public|BlueprintEvent) // @ game+0x1a5c6b0
	void K2_OnEndViewTarget(struct APlayerController* PC);																																																																	   // Function Engine.Actor.K2_OnEndViewTarget // (Event|Public|BlueprintEvent) // @ game+0x1a5c6b0
	void K2_OnBecomeViewTarget(struct APlayerController* PC);																																																																   // Function Engine.Actor.K2_OnBecomeViewTarget // (Event|Public|BlueprintEvent) // @ game+0x1a5c6b0
	struct USceneComponent* K2_GetRootComponent();																																																																			   // Function Engine.Actor.K2_GetRootComponent // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x368bc20
	struct TArray<struct UActorComponent*> K2_GetComponentsByClass(struct UActorComponent* ComponentClass);																																																					   // Function Engine.Actor.K2_GetComponentsByClass // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x368bb40
	struct FRotator K2_GetActorRotation();																																																																					   // Function Engine.Actor.K2_GetActorRotation // (Final|Native|Public|HasDefaults|BlueprintCallable|BlueprintPure|Const) // @ game+0x368ba80
	struct FVector K2_GetActorLocation();																																																																					   // Function Engine.Actor.K2_GetActorLocation // (Final|Native|Public|HasDefaults|BlueprintCallable|BlueprintPure|Const) // @ game+0x368ba00
	void K2_DetachFromActor(enum class EDetachmentRule LocationRule, enum class EDetachmentRule RotationRule, enum class EDetachmentRule ScaleRule);																																										   // Function Engine.Actor.K2_DetachFromActor // (Final|Native|Public|BlueprintCallable) // @ game+0x368b8f0
	void K2_DestroyComponent(struct UActorComponent* Component);																																																															   // Function Engine.Actor.K2_DestroyComponent // (Final|Native|Public|BlueprintCallable) // @ game+0x368b860
	void K2_DestroyActor();																																																																									   // Function Engine.Actor.K2_DestroyActor // (Native|Public|BlueprintCallable) // @ game+0x368b840
	void K2_AttachToComponent(struct USceneComponent* Parent, struct FName SocketName, enum class EAttachmentRule LocationRule, enum class EAttachmentRule RotationRule, enum class EAttachmentRule ScaleRule, bool bWeldSimulatedBodies);																					   // Function Engine.Actor.K2_AttachToComponent // (Final|Native|Public|BlueprintCallable) // @ game+0x368b670
	void K2_AttachToActor(struct AActor* ParentActor, struct FName SocketName, enum class EAttachmentRule LocationRule, enum class EAttachmentRule RotationRule, enum class EAttachmentRule ScaleRule, bool bWeldSimulatedBodies);																							   // Function Engine.Actor.K2_AttachToActor // (Final|Native|Public|BlueprintCallable) // @ game+0x368b4a0
	void K2_AttachRootComponentToActor(struct AActor* InParentActor, struct FName InSocketName, enum class EAttachLocation AttachLocationType, bool bWeldSimulatedBodies);																																					   // Function Engine.Actor.K2_AttachRootComponentToActor // (Final|Native|Public|BlueprintCallable) // @ game+0x368b340
	void K2_AttachRootComponentTo(struct USceneComponent* InParent, struct FName InSocketName, enum class EAttachLocation AttachLocationType, bool bWeldSimulatedBodies);																																					   // Function Engine.Actor.K2_AttachRootComponentTo // (Final|Native|Public|BlueprintCallable) // @ game+0x368b1e0
	void K2_AddActorWorldTransformKeepScale(struct FTransform& DeltaTransform, bool bSweep, struct FHitResult& SweepHitResult, bool bTeleport);																																												   // Function Engine.Actor.K2_AddActorWorldTransformKeepScale // (Final|Native|Public|HasOutParms|HasDefaults|BlueprintCallable) // @ game+0x368afd0
	void K2_AddActorWorldTransform(struct FTransform& DeltaTransform, bool bSweep, struct FHitResult& SweepHitResult, bool bTeleport);																																														   // Function Engine.Actor.K2_AddActorWorldTransform // (Final|Native|Public|HasOutParms|HasDefaults|BlueprintCallable) // @ game+0x368adc0
	void K2_AddActorWorldRotation(struct FRotator DeltaRotation, bool bSweep, struct FHitResult& SweepHitResult, bool bTeleport);																																															   // Function Engine.Actor.K2_AddActorWorldRotation // (Final|Native|Public|HasOutParms|HasDefaults|BlueprintCallable) // @ game+0x368ac10
	void K2_AddActorWorldOffset(struct FVector DeltaLocation, bool bSweep, struct FHitResult& SweepHitResult, bool bTeleport);																																																   // Function Engine.Actor.K2_AddActorWorldOffset // (Final|Native|Public|HasOutParms|HasDefaults|BlueprintCallable) // @ game+0x368aa60
	void K2_AddActorLocalTransform(struct FTransform& NewTransform, bool bSweep, struct FHitResult& SweepHitResult, bool bTeleport);																																														   // Function Engine.Actor.K2_AddActorLocalTransform // (Final|Native|Public|HasOutParms|HasDefaults|BlueprintCallable) // @ game+0x368a850
	void K2_AddActorLocalRotation(struct FRotator DeltaRotation, bool bSweep, struct FHitResult& SweepHitResult, bool bTeleport);																																															   // Function Engine.Actor.K2_AddActorLocalRotation // (Final|Native|Public|HasOutParms|HasDefaults|BlueprintCallable) // @ game+0x368a6a0
	void K2_AddActorLocalOffset(struct FVector DeltaLocation, bool bSweep, struct FHitResult& SweepHitResult, bool bTeleport);																																																   // Function Engine.Actor.K2_AddActorLocalOffset // (Final|Native|Public|HasOutParms|HasDefaults|BlueprintCallable) // @ game+0x368a4f0
	bool IsOverlappingActor(struct AActor* Other);																																																																			   // Function Engine.Actor.IsOverlappingActor // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x368a450
	bool IsChildActor();																																																																									   // Function Engine.Actor.IsChildActor // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x368a420
	bool IsActorTickEnabled();																																																																								   // Function Engine.Actor.IsActorTickEnabled // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x368a3f0
	bool IsActorBeingDestroyed();																																																																							   // Function Engine.Actor.IsActorBeingDestroyed // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x368a3c0
	bool HasAuthority();																																																																									   // Function Engine.Actor.HasAuthority // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x368a390
	float GetVerticalDistanceTo(struct AActor* OtherActor);																																																																	   // Function Engine.Actor.GetVerticalDistanceTo // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x368a2f0
	struct FVector GetVelocity();																																																																							   // Function Engine.Actor.GetVelocity // (Native|Public|HasDefaults|BlueprintCallable|BlueprintPure|Const) // @ game+0x368a2b0
	struct FTransform GetTransform();																																																																						   // Function Engine.Actor.GetTransform // (Final|Native|Public|HasDefaults|BlueprintCallable|BlueprintPure|Const) // @ game+0x368a260
	bool GetTickableWhenPaused();																																																																							   // Function Engine.Actor.GetTickableWhenPaused // (Final|Native|Public|BlueprintCallable) // @ game+0x368a230
	float GetSquaredHorizontalDistanceTo(struct AActor* OtherActor);																																																														   // Function Engine.Actor.GetSquaredHorizontalDistanceTo // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x368a190
	float GetSquaredDistanceTo(struct AActor* OtherActor);																																																																	   // Function Engine.Actor.GetSquaredDistanceTo // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x368a0f0
	enum class ENetRole GetRemoteRole();																																																																					   // Function Engine.Actor.GetRemoteRole // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x368a0d0
	struct UChildActorComponent* GetParentComponent();																																																																		   // Function Engine.Actor.GetParentComponent // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x368a0a0
	struct AActor* GetParentActor();																																																																						   // Function Engine.Actor.GetParentActor // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x368a070
	struct AActor* GetOwner();																																																																								   // Function Engine.Actor.GetOwner // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x368a050
	void GetOverlappingComponents(struct TArray<struct UPrimitiveComponent*>& OverlappingComponents);																																																						   // Function Engine.Actor.GetOverlappingComponents // (Final|Native|Public|HasOutParms|BlueprintCallable|BlueprintPure|Const) // @ game+0x3689fa0
	void GetOverlappingActors(struct TArray<struct AActor*>& OverlappingActors, struct AActor* ClassFilter);																																																				   // Function Engine.Actor.GetOverlappingActors // (Final|Native|Public|HasOutParms|BlueprintCallable|BlueprintPure|Const) // @ game+0x3689ea0
	enum class ENetRole GetLocalRole();																																																																						   // Function Engine.Actor.GetLocalRole // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x3689e80
	float GetLifeSpan();																																																																									   // Function Engine.Actor.GetLifeSpan // (Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x3689e40
	struct AController* GetInstigatorController();																																																																			   // Function Engine.Actor.GetInstigatorController // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x3689e10
	struct APawn* GetInstigator();																																																																							   // Function Engine.Actor.GetInstigator // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x3689de0
	struct FVector GetInputVectorAxisValue(struct FKey InputAxisKey);																																																														   // Function Engine.Actor.GetInputVectorAxisValue // (Final|Native|Public|HasDefaults|BlueprintCallable|BlueprintPure|Const) // @ game+0x3689cd0
	float GetInputAxisValue(struct FName InputAxisName);																																																																	   // Function Engine.Actor.GetInputAxisValue // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x3689c30
	float GetInputAxisKeyValue(struct FKey InputAxisKey);																																																																	   // Function Engine.Actor.GetInputAxisKeyValue // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x3689b30
	float GetHorizontalDotProductTo(struct AActor* OtherActor);																																																																   // Function Engine.Actor.GetHorizontalDotProductTo // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x3689a90
	float GetHorizontalDistanceTo(struct AActor* OtherActor);																																																																   // Function Engine.Actor.GetHorizontalDistanceTo // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x36899f0
	float GetGameTimeSinceCreation();																																																																						   // Function Engine.Actor.GetGameTimeSinceCreation // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x36899c0
	float GetDotProductTo(struct AActor* OtherActor);																																																																		   // Function Engine.Actor.GetDotProductTo // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x3689920
	float GetDistanceTo(struct AActor* OtherActor);																																																																			   // Function Engine.Actor.GetDistanceTo // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x3689880
	struct TArray<struct UActorComponent*> GetComponentsByTag(struct UActorComponent* ComponentClass, struct FName Tag);																																																	   // Function Engine.Actor.GetComponentsByTag // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x3689770
	struct TArray<struct UActorComponent*> GetComponentsByInterface(struct UInterface* Interface);																																																							   // Function Engine.Actor.GetComponentsByInterface // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x3689690
	struct UActorComponent* GetComponentByClass(struct UActorComponent* ComponentClass);																																																									   // Function Engine.Actor.GetComponentByClass // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x36895f0
	struct FName GetAttachParentSocketName();																																																																				   // Function Engine.Actor.GetAttachParentSocketName // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x36894b0
	struct AActor* GetAttachParentActor();																																																																					   // Function Engine.Actor.GetAttachParentActor // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x3689480
	void GetAttachedActors(struct TArray<struct AActor*>& OutActors, bool bResetArray);																																																										   // Function Engine.Actor.GetAttachedActors // (Final|Native|Public|HasOutParms|BlueprintCallable|BlueprintPure|Const) // @ game+0x36894f0
	void GetAllChildActors(struct TArray<struct AActor*>& ChildActors, bool bIncludeDescendants);																																																							   // Function Engine.Actor.GetAllChildActors // (Final|Native|Public|HasOutParms|BlueprintCallable|BlueprintPure|Const) // @ game+0x3689380
	struct FVector GetActorUpVector();																																																																						   // Function Engine.Actor.GetActorUpVector // (Final|Native|Public|HasDefaults|BlueprintCallable|BlueprintPure|Const) // @ game+0x3689310
	float GetActorTimeDilation();																																																																							   // Function Engine.Actor.GetActorTimeDilation // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x36892e0
	float GetActorTickInterval();																																																																							   // Function Engine.Actor.GetActorTickInterval // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x36892b0
	struct FVector GetActorScale3D();																																																																						   // Function Engine.Actor.GetActorScale3D // (Final|Native|Public|HasDefaults|BlueprintCallable|BlueprintPure|Const) // @ game+0x3689270
	struct FVector GetActorRightVector();																																																																					   // Function Engine.Actor.GetActorRightVector // (Final|Native|Public|HasDefaults|BlueprintCallable|BlueprintPure|Const) // @ game+0x3689200
	struct FVector GetActorRelativeScale3D();																																																																				   // Function Engine.Actor.GetActorRelativeScale3D // (Final|Native|Public|HasDefaults|BlueprintCallable|BlueprintPure|Const) // @ game+0x36891c0
	struct FVector GetActorForwardVector();																																																																					   // Function Engine.Actor.GetActorForwardVector // (Final|Native|Public|HasDefaults|BlueprintCallable|BlueprintPure|Const) // @ game+0x3689150
	void GetActorEyesViewPoint(struct FVector& OutLocation, struct FRotator& OutRotation);																																																									   // Function Engine.Actor.GetActorEyesViewPoint // (Native|Public|HasOutParms|HasDefaults|BlueprintCallable|BlueprintPure|Const) // @ game+0x3689060
	bool GetActorEnableCollision();																																																																							   // Function Engine.Actor.GetActorEnableCollision // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x3689030
	void GetActorBounds(bool bOnlyCollidingComponents, struct FVector& Origin, struct FVector& BoxExtent, bool bIncludeFromChildActors);																																													   // Function Engine.Actor.GetActorBounds // (Final|Native|Public|HasOutParms|HasDefaults|BlueprintCallable|BlueprintPure|Const) // @ game+0x3688ea0
	void ForceNetUpdate();																																																																									   // Function Engine.Actor.ForceNetUpdate // (Native|Public|BlueprintCallable) // @ game+0x1664bc0
	void FlushNetDormancy();																																																																								   // Function Engine.Actor.FlushNetDormancy // (Final|BlueprintAuthorityOnly|Native|Public|BlueprintCallable) // @ game+0x3688e80
	void FinishAddComponent(struct UActorComponent* Component, bool bManualAttachment, struct FTransform& RelativeTransform);																																																   // Function Engine.Actor.FinishAddComponent // (Final|Native|Public|HasOutParms|HasDefaults|BlueprintCallable) // @ game+0x3688d20
	void EnableInput(struct APlayerController* PlayerController);																																																															   // Function Engine.Actor.EnableInput // (Native|Public|BlueprintCallable) // @ game+0x166fca0
	void DisableInput(struct APlayerController* PlayerController);																																																															   // Function Engine.Actor.DisableInput // (Native|Public|BlueprintCallable) // @ game+0x3688c90
	void DetachRootComponentFromParent(bool bMaintainWorldPosition);																																																														   // Function Engine.Actor.DetachRootComponentFromParent // (Final|Native|Public|BlueprintCallable) // @ game+0x3688c00
	void AddTickPrerequisiteComponent(struct UActorComponent* PrerequisiteComponent);																																																										   // Function Engine.Actor.AddTickPrerequisiteComponent // (Native|Public|BlueprintCallable) // @ game+0x3688b70
	void AddTickPrerequisiteActor(struct AActor* PrerequisiteActor);																																																														   // Function Engine.Actor.AddTickPrerequisiteActor // (Native|Public|BlueprintCallable) // @ game+0x3688ae0
	struct UActorComponent* AddComponentByClass(struct UActorComponent* Class, bool bManualAttachment, struct FTransform& RelativeTransform, bool bDeferredFinish);																																							   // Function Engine.Actor.AddComponentByClass // (Final|Native|Public|HasOutParms|HasDefaults|BlueprintCallable) // @ game+0x3688920
	struct UActorComponent* AddComponent(struct FName TemplateName, bool bManualAttachment, struct FTransform& RelativeTransform, struct UObject* ComponentTemplateContext, bool bDeferredFinish);																															   // Function Engine.Actor.AddComponent // (Final|Native|Public|HasOutParms|HasDefaults|BlueprintCallable) // @ game+0x3688710
	bool ActorHasTag(struct FName Tag);																																																																						   // Function Engine.Actor.ActorHasTag // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x3688670
};
