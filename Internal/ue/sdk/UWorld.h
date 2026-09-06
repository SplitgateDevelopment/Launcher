#pragma once

#include "Fwd.h"
#include "Values.h"
#include "UObject.h"

// Class Engine.World
// Size: 0x798 (Inherited: 0x28)
struct UWorld : UObject
{
	char pad_28[0x8];																		  // 0x28(0x08)
	struct ULevel* PersistentLevel;															  // 0x30(0x08)
	struct UNetDriver* NetDriver;															  // 0x38(0x08)
	struct ULineBatchComponent* LineBatcher;												  // 0x40(0x08)
	struct ULineBatchComponent* PersistentLineBatcher;										  // 0x48(0x08)
	struct ULineBatchComponent* ForegroundLineBatcher;										  // 0x50(0x08)
	struct AGameNetworkManager* NetworkManager;												  // 0x58(0x08)
	struct UPhysicsCollisionHandler* PhysicsCollisionHandler;								  // 0x60(0x08)
	struct TArray<struct UObject*> ExtraReferencedObjects;									  // 0x68(0x10)
	struct TArray<struct UObject*> PerModuleDataObjects;									  // 0x78(0x10)
	struct TArray<struct ULevelStreaming*> StreamingLevels;									  // 0x88(0x10)
	char StreamingLevelsToConsider[0x28];													  // 0x98(0x28)
	struct FString StreamingLevelsPrefix;													  // 0xc0(0x10)
	struct ULevel* CurrentLevelPendingVisibility;											  // 0xd0(0x08)
	struct ULevel* CurrentLevelPendingInvisibility;											  // 0xd8(0x08)
	struct UDemoNetDriver* DemoNetDriver;													  // 0xe0(0x08)
	struct AParticleEventManager* MyParticleEventManager;									  // 0xe8(0x08)
	struct APhysicsVolume* DefaultPhysicsVolume;											  // 0xf0(0x08)
	char pad_F8[0x16];																		  // 0xf8(0x16)
	char pad_10E_0 : 2;																		  // 0x10e(0x01)
	char bAreConstraintsDirty : 1;															  // 0x10e(0x01)
	char pad_10E_3 : 5;																		  // 0x10e(0x01)
	char pad_10F[0x1];																		  // 0x10f(0x01)
	struct UNavigationSystemBase* NavigationSystem;											  // 0x110(0x08)
	struct AGameModeBase* AuthorityGameMode;												  // 0x118(0x08)
	struct AGameStateBase* GameState;														  // 0x120(0x08)
	struct UAISystemBase* AISystem;															  // 0x128(0x08)
	struct UAvoidanceManager* AvoidanceManager;												  // 0x130(0x08)
	struct TArray<struct ULevel*> Levels;													  // 0x138(0x10)
	struct TArray<struct FLevelCollection> LevelCollections;								  // 0x148(0x10)
	char pad_158[0x28];																		  // 0x158(0x28)
	struct UGameInstance* OwningGameInstance;												  // 0x180(0x08)
	struct TArray<struct UMaterialParameterCollectionInstance*> ParameterCollectionInstances; // 0x188(0x10)
	struct UCanvas* CanvasForRenderingToTarget;												  // 0x198(0x08)
	struct UCanvas* CanvasForDrawMaterialToRenderTarget;									  // 0x1a0(0x08)
	char pad_1A8[0x50];																		  // 0x1a8(0x50)
	struct UPhysicsFieldComponent* PhysicsField;											  // 0x1f8(0x08)
	char ComponentsThatNeedPreEndOfFrameSync[0x50];											  // 0x200(0x50)
	struct TArray<struct UActorComponent*> ComponentsThatNeedEndOfFrameUpdate;				  // 0x250(0x10)
	struct TArray<struct UActorComponent*> ComponentsThatNeedEndOfFrameUpdate_OnGameThread;	  // 0x260(0x10)
	char pad_270[0x370];																	  // 0x270(0x370)
	struct UWorldComposition* WorldComposition;												  // 0x5e0(0x08)
	char pad_5E8[0x90];																		  // 0x5e8(0x90)
	char PSCPool[0x58];																		  // 0x678(0x58)
	char pad_6D0[0xc8];																		  // 0x6d0(0xc8)

	struct AWorldSettings* K2_GetWorldSettings(); // Function Engine.World.K2_GetWorldSettings // (Final|Native|Public|BlueprintCallable) // @ game+0x3809c70
	void HandleTimelineScrubbed();				  // Function Engine.World.HandleTimelineScrubbed // (Final|Native|Public) // @ game+0x3809c50

	static struct UWorld* GetWorld();
	static struct UClass* StaticClass();
	static struct UWorld* GetDefaultObj();
};
