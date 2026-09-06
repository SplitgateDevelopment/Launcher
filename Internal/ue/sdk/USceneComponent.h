#pragma once

#include "Fwd.h"
#include "Values.h"
#include "UActorComponent.h"

// Class Engine.SceneComponent
// Size: 0x200 (Inherited: 0xb0)
struct USceneComponent : UActorComponent
{
	char pad_B0[0x8];											   // 0xb0(0x08)
	char PhysicsVolume[0x08];									   // 0xb8(0x08)
	struct USceneComponent* AttachParent;						   // 0xc0(0x08)
	struct FName AttachSocketName;								   // 0xc8(0x08)
	struct TArray<struct USceneComponent*> AttachChildren;		   // 0xd0(0x10)
	struct TArray<struct USceneComponent*> ClientAttachedChildren; // 0xe0(0x10)
	char pad_F0[0x2c];											   // 0xf0(0x2c)
	struct FVector RelativeLocation;							   // 0x11c(0x0c)
	struct FRotator RelativeRotation;							   // 0x128(0x0c)
	struct FVector RelativeScale3D;								   // 0x134(0x0c)
	struct FVector ComponentVelocity;							   // 0x140(0x0c)
	char bComponentToWorldUpdated : 1;							   // 0x14c(0x01)
	char pad_14C_1 : 1;											   // 0x14c(0x01)
	char bAbsoluteLocation : 1;									   // 0x14c(0x01)
	char bAbsoluteRotation : 1;									   // 0x14c(0x01)
	char bAbsoluteScale : 1;									   // 0x14c(0x01)
	char bVisible : 1;											   // 0x14c(0x01)
	char bShouldBeAttached : 1;									   // 0x14c(0x01)
	char bShouldSnapLocationWhenAttached : 1;					   // 0x14c(0x01)
	char bShouldSnapRotationWhenAttached : 1;					   // 0x14d(0x01)
	char bShouldUpdatePhysicsVolume : 1;						   // 0x14d(0x01)
	char bHiddenInGame : 1;										   // 0x14d(0x01)
	char bBoundsChangeTriggersStreamingDataRebuild : 1;			   // 0x14d(0x01)
	char bUseAttachParentBound : 1;								   // 0x14d(0x01)
	char pad_14D_5 : 3;											   // 0x14d(0x01)
	char pad_14E[0x1];											   // 0x14e(0x01)
	char Mobility[0x01];										   // 0x14f(0x01)
	char DetailMode[0x01];										   // 0x150(0x01)
	char PhysicsVolumeChangedDelegate[0x01];					   // 0x151(0x01)
	char pad_152[0xae];											   // 0x152(0xae)

	void K2_SetRelativeRotation(struct FRotator NewRotation, bool bSweep, struct FHitResult& SweepHitResult, bool bTeleport); // Function Engine.SceneComponent.K2_SetRelativeRotation // (Final|Native|Public|HasOutParms|HasDefaults|BlueprintCallable) // @ game+0x37d0da0
};
