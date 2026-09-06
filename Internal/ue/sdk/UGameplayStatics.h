#pragma once

#include "Fwd.h"
#include "Values.h"
#include "UBlueprintFunctionLibrary.h"

// Class Engine.GameplayStatics
// Size: 0x28 (Inherited: 0x28)
struct UGameplayStatics : UBlueprintFunctionLibrary
{
	struct UObject* SpawnObject(struct UObject* ObjectClass, struct UObject* Outer); // Function Engine.GameplayStatics.SpawnObject // (Final|Native|Static|Public|BlueprintCallable) // @ game+0x36f9330

	// Hand-added (not in the dump). The deferred spawn two-step; call on the CDO
	// (Engine::GameplayStatics). See SpawnActor() for the convenience wrapper.
	struct AActor* BeginDeferredActorSpawnFromClass(struct UObject* WorldContextObject, struct UClass* ActorClass, struct FTransform SpawnTransform, ESpawnActorCollisionHandlingMethod CollisionHandlingOverride, struct AActor* Owner);
	struct AActor* FinishSpawningActor(struct AActor* Actor, struct FTransform SpawnTransform);

	static struct UClass* StaticClass();
};
