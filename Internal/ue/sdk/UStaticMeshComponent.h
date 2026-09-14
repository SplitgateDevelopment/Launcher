#pragma once

#include "Fwd.h"
#include "Values.h"
#include "UMeshComponent.h"

// Class Engine.StaticMeshComponent
// Size: 0x4e0 (Inherited: 0x480)
struct UStaticMeshComponent : UMeshComponent
{
	int32_t ForcedLodModel;												   // 0x478(0x04)
	int32_t PreviousLODLevel;											   // 0x47c(0x04)
	int32_t MinLOD;														   // 0x480(0x04)
	int32_t SubDivisionStepSize;										   // 0x484(0x04)
	struct UStaticMesh* StaticMesh;										   // 0x488(0x08)
	struct FColor WireframeColorOverride;								   // 0x490(0x04)
	char bEvaluateWorldPositionOffset : 1;								   // 0x494(0x01)
	char bOverrideWireframeColor : 1;									   // 0x494(0x01)
	char bOverrideMinLod : 1;											   // 0x494(0x01)
	char bOverrideNavigationExport : 1;									   // 0x494(0x01)
	char bForceNavigationObstacle : 1;									   // 0x494(0x01)
	char bDisallowMeshPaintPerInstance : 1;								   // 0x494(0x01)
	char bIgnoreInstanceForTextureStreaming : 1;						   // 0x494(0x01)
	char bOverrideLightMapRes : 1;										   // 0x494(0x01)
	char bCastDistanceFieldIndirectShadow : 1;							   // 0x495(0x01)
	char bOverrideDistanceFieldSelfShadowBias : 1;						   // 0x495(0x01)
	char bUseSubDivisions : 1;											   // 0x495(0x01)
	char bUseDefaultCollision : 1;										   // 0x495(0x01)
	char bReverseCulling : 1;											   // 0x495(0x01)
	int32_t OverriddenLightMapRes;										   // 0x498(0x04)
	float DistanceFieldIndirectShadowMinVisibility;						   // 0x49c(0x04)
	float DistanceFieldSelfShadowBias;									   // 0x4a0(0x04)
	float StreamingDistanceMultiplier;									   // 0x4a4(0x04)
	struct TArray<struct FStaticMeshComponentLODInfo> LODData;			   // 0x4a8(0x10)
	struct TArray<struct FStreamingTextureBuildInfo> StreamingTextureData; // 0x4b8(0x10)
	char LightmassSettings[0x18];										   // 0x4c8(0x18)

	bool SetStaticMesh(struct UStaticMesh* NewMesh);				// Function Engine.StaticMeshComponent.SetStaticMesh // (Native|Public|BlueprintCallable) // @ game+0x37fb970
	void SetReverseCulling(bool ReverseCulling);					// Function Engine.StaticMeshComponent.SetReverseCulling // (Final|Native|Public|BlueprintCallable) // @ game+0x37fb720
	void SetForcedLodModel(int32_t NewForcedLodModel);				// Function Engine.StaticMeshComponent.SetForcedLodModel // (Final|Native|Public|BlueprintCallable) // @ game+0x37fb2e0
	void SetEvaluateWorldPositionOffsetInRayTracing(bool NewValue); // Function Engine.StaticMeshComponent.SetEvaluateWorldPositionOffsetInRayTracing // (Final|Native|Public|BlueprintCallable) // @ game+0x37fb250
	void SetDistanceFieldSelfShadowBias(float NewValue);			// Function Engine.StaticMeshComponent.SetDistanceFieldSelfShadowBias // (Final|Native|Public|BlueprintCallable) // @ game+0x37fafd0
	void OnRep_StaticMesh(struct UStaticMesh* OldStaticMesh);		// Function Engine.StaticMeshComponent.OnRep_StaticMesh // (Final|Native|Public) // @ game+0x37faf40
	void GetLocalBounds(struct FVector& Min, struct FVector& Max);	// Function Engine.StaticMeshComponent.GetLocalBounds // (Final|Native|Public|HasOutParms|HasDefaults|BlueprintCallable|BlueprintPure|Const) // @ game+0x37fada0
};
