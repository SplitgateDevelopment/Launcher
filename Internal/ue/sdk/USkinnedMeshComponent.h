#pragma once

#include "Fwd.h"
#include "Values.h"
#include "UMeshComponent.h"

// Class Engine.SkinnedMeshComponent
// Size: 0x6a0 (Inherited: 0x480)
struct USkinnedMeshComponent : UMeshComponent
{
	struct USkeletalMesh* SkeletalMesh;							// 0x480(0x08)
	char MasterPoseComponent[0x08];								// 0x488(0x08)
	struct TArray<enum struct ESkinCacheUsage> SkinCacheUsage;	// 0x490(0x10)
	struct TArray<struct FVertexOffsetUsage> VertexOffsetUsage; // 0x4a0(0x10)
	char pad_4B0[0xf8];											// 0x4b0(0xf8)
	struct UPhysicsAsset* PhysicsAssetOverride;					// 0x5a8(0x08)
	int32_t ForcedLodModel;										// 0x5b0(0x04)
	int32_t MinLodModel;										// 0x5b4(0x04)
	char pad_5B8[0x8];											// 0x5b8(0x08)
	float StreamingDistanceMultiplier;							// 0x5c0(0x04)
	char pad_5C4[0xc];											// 0x5c4(0x0c)
	struct TArray<struct FSkelMeshComponentLODInfo> LODInfo;	// 0x5d0(0x10)
	char pad_5E0[0x24];											// 0x5e0(0x24)
	char VisibilityBasedAnimTickOption[0x01];					// 0x604(0x01)
	char pad_605[0x1];											// 0x605(0x01)
	char pad_606_0 : 3;											// 0x606(0x01)
	char bOverrideMinLod : 1;									// 0x606(0x01)
	char bUseBoundsFromMasterPoseComponent : 1;					// 0x606(0x01)
	char bForceWireframe : 1;									// 0x606(0x01)
	char bDisplayBones : 1;										// 0x606(0x01)
	char bDisableMorphTarget : 1;								// 0x606(0x01)
	char bHideSkin : 1;											// 0x607(0x01)
	char bPerBoneMotionBlur : 1;								// 0x607(0x01)
	char bComponentUseFixedSkelBounds : 1;						// 0x607(0x01)
	char bConsiderAllBodiesForBounds : 1;						// 0x607(0x01)
	char bSyncAttachParentLOD : 1;								// 0x607(0x01)
	char bCanHighlightSelectedSections : 1;						// 0x607(0x01)
	char bRecentlyRendered : 1;									// 0x607(0x01)
	char bCastCapsuleDirectShadow : 1;							// 0x607(0x01)
	char bCastCapsuleIndirectShadow : 1;						// 0x608(0x01)
	char bCPUSkinning : 1;										// 0x608(0x01)
	char bEnableUpdateRateOptimizations : 1;					// 0x608(0x01)
	char bDisplayDebugUpdateRateOptimizations : 1;				// 0x608(0x01)
	char bRenderStatic : 1;										// 0x608(0x01)
	char bIgnoreMasterPoseComponentLOD : 1;						// 0x608(0x01)
	char pad_608_6 : 2;											// 0x608(0x01)
	char bCachedLocalBoundsUpToDate : 1;						// 0x609(0x01)
	char pad_609_1 : 1;											// 0x609(0x01)
	char bForceMeshObjectUpdate : 1;							// 0x609(0x01)
	char pad_609_3 : 5;											// 0x609(0x01)
	char pad_60A[0x2];											// 0x60a(0x02)
	float CapsuleIndirectShadowMinVisibility;					// 0x60c(0x04)
	char pad_610[0x10];											// 0x610(0x10)
	char CachedWorldSpaceBounds[0x1c];							// 0x620(0x1c)
	char pad_63C[0x4];											// 0x63c(0x04)
	struct FMatrix CachedWorldToLocalTransform;					// 0x640(0x40)
	char pad_680[0x20];											// 0x680(0x20)

	struct FName GetBoneName(int32_t BoneIndex); // Function Engine.SkinnedMeshComponent.GetBoneName // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x37dfd40
};
