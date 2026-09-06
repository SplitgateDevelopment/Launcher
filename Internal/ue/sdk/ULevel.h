#pragma once

#include "Fwd.h"
#include "Values.h"
#include "UObject.h"

// Class Engine.Level
// Size: 0x298 (Inherited: 0x28)
struct ULevel : UObject
{
	uint8_t pad0[0x70];
	TArray<struct AActor*> Actors;
	TArray<struct AActor*> GCActors;
	struct UWorld* OwningWorld;																	 // 0xb8(0x08)
	struct UModel* Model;																		 // 0xc0(0x08)
	struct TArray<struct UModelComponent*> ModelComponents;										 // 0xc8(0x10)
	struct ULevelActorContainer* ActorCluster;													 // 0xd8(0x08)
	int32_t NumTextureStreamingUnbuiltComponents;												 // 0xe0(0x04)
	int32_t NumTextureStreamingDirtyResources;													 // 0xe4(0x04)
	struct ALevelScriptActor* LevelScriptActor;													 // 0xe8(0x08)
	struct ANavigationObjectBase* NavListStart;													 // 0xf0(0x08)
	struct ANavigationObjectBase* NavListEnd;													 // 0xf8(0x08)
	struct TArray<struct UNavigationDataChunk*> NavDataChunks;									 // 0x100(0x10)
	float LightmapTotalSize;																	 // 0x110(0x04)
	float ShadowmapTotalSize;																	 // 0x114(0x04)
	struct TArray<struct FVector> StaticNavigableGeometry;										 // 0x118(0x10)
	struct TArray<struct FGuid> StreamingTextureGuids;											 // 0x128(0x10)
	char pad_138[0x98];																			 // 0x138(0x98)
	char LevelBuildDataId[0x10];																 // 0x1d0(0x10)
	struct UMapBuildDataRegistry* MapBuildData;													 // 0x1e0(0x08)
	char LightBuildLevelOffset[0x0c];															 // 0x1e8(0x0c)
	char bIsLightingScenario : 1;																 // 0x1f4(0x01)
	char pad_1F4_1 : 2;																			 // 0x1f4(0x01)
	char bTextureStreamingRotationChanged : 1;													 // 0x1f4(0x01)
	char bStaticComponentsRegisteredInStreamingManager : 1;										 // 0x1f4(0x01)
	char bIsVisible : 1;																		 // 0x1f4(0x01)
	char pad_1F4_6 : 2;																			 // 0x1f4(0x01)
	char pad_1F5[0x63];																			 // 0x1f5(0x63)
	struct AWorldSettings* WorldSettings;														 // 0x258(0x08)
	char pad_260[0x8];																			 // 0x260(0x08)
	struct TArray<struct UAssetUserData*> AssetUserData;										 // 0x268(0x10)
	char pad_278[0x10];																			 // 0x278(0x10)
	struct TArray<struct FReplicatedStaticActorDestructionInfo> DestroyedReplicatedStaticActors; // 0x288(0x10)
};
