/// @file
/// @brief Hand-written helpers: free engine functions and the convenience
/// member methods added onto the generated SDK types (accessors, name lookup, spawn).

#include "Engine.h"

using namespace Engine;

std::string FNameEntry::String()
{
	if (bIsWide)
	{
		return std::string();
	}
	return {AnsiName, Len};
}
FNameEntry* FNamePool::GetEntry(FNameEntryHandle handle) const
{
	return reinterpret_cast<FNameEntry*>(Blocks[handle.Block] + 2 * static_cast<uint64_t>(handle.Offset));
}
std::string FName::GetName()
{
	auto entry = NamePoolData->GetEntry(Index);
	auto name = entry->String();
	if (Number > 0)
	{
		name += '_' + std::to_string(Number);
	}
	auto pos = name.rfind('/');
	if (pos != std::string::npos)
	{
		name = name.substr(pos + 1);
	}
	return name;
}
std::string UObject::GetName()
{
	return NamePrivate.GetName();
}
std::string UObject::GetFullName()
{
	std::string name;
	for (auto outer = OuterPrivate; outer; outer = outer->OuterPrivate)
	{
		name = outer->GetName() + "." + name;
	}
	name = ClassPrivate->GetName() + " " + name + this->GetName();
	return name;
}
bool UObject::IsA(void* cmp)
{
	for (auto super = ClassPrivate; super; super = static_cast<UClass*>(super->SuperStruct))
	{
		if (super == cmp)
		{
			return true;
		}
	}
	return false;
}
bool UObject::IsDefaultObject() const
{
	return (ObjectFlags & 0x10) == 0x10;
}
UObject* TUObjectArray::GetObjectPtr(uint32_t id) const
{
	if (id >= NumElements) return nullptr;
	uint64_t chunkIndex = id / 65536;
	if (chunkIndex >= NumChunks) return nullptr;
	auto chunk = Objects[chunkIndex];
	if (!chunk) return nullptr;
	uint32_t withinChunkIndex = id % 65536 * 24;
	auto item = *reinterpret_cast<UObject**>(chunk + withinChunkIndex);
	return item;
}
UObject* TUObjectArray::FindObject(const char* name) const
{
	for (auto i = 0u; i < NumElements; i++)
	{
		auto object = GetObjectPtr(i);
		if (object && object->GetFullName() == name)
		{
			return object;
		}
	}
	return nullptr;
}
void UObject::ProcessEvent(void* UFunction, void* Params)
{
	auto vtable = *reinterpret_cast<void***>(this);
	reinterpret_cast<void (*)(void*, void*, void*)>(vtable[68])(this, UFunction, Params);
}
struct UClass* UObject::StaticClass()
{
	static struct UClass* Clss = nullptr;

	if (!Clss)
		Clss = (UClass*)ObjObjects->FindObject("Class CoreUObject.Object");

	return Clss;
}
UObject* UObject::GetDefaultObj()
{
	static struct UObject* Default = nullptr;

	if (!Default)
		Default = static_cast<UObject*>(UObject::StaticClass());

	return Default;
}
UPortalWarsSaveGame* UPortalWarsLocalPlayer::GetUserSaveGame()
{
	struct
	{
		UPortalWarsSaveGame* ReturnValue;
	} Parameters;

	static auto Function = ObjObjects->FindObject("Function PortalWars.PortalWarsLocalPlayer.GetUserSaveGame");
	ProcessEvent(Function, &Parameters);
	return Parameters.ReturnValue;
};
struct UClass* UEngine::StaticClass()
{
	return (UClass*)ObjObjects->FindObject("Class Engine.Engine");
}
struct UEngine* UEngine::GetEngine()
{
	static UEngine* GEngine = nullptr;
	static UClass* EngineClass = UEngine::StaticClass();

	if (!GEngine)
	{
		for (auto i = 0u; i < ObjObjects->NumElements; i++)
		{
			auto object = ObjObjects->GetObjectPtr(i);

			if (!object)
				continue;

			if (object->IsA(EngineClass) && !object->IsDefaultObject())
			{
				GEngine = static_cast<UEngine*>(object);
				break;
			}
		}
	}

	return GEngine;
}
struct UWorld* UWorld::GetWorld()
{
	UWorld* World = *(UWorld**)(WRLD);
	return World;
}
struct UClass* UInputSettings::StaticClass()
{
	return (UClass*)ObjObjects->FindObject("Class Engine.InputSettings");
}
struct UInputSettings* UInputSettings::GetDefaultObj()
{
	static struct UInputSettings* Default = nullptr;

	if (!Default)
		Default = static_cast<UInputSettings*>((UObject*)UInputSettings::StaticClass());

	return Default;
}
struct UInputSettings* UInputSettings::GetInputSettings()
{
	auto Func = ObjObjects->FindObject("Function Engine.InputSettings.GetInputSettings");

	struct
	{
		struct UInputSettings* ReturnValue;
	} Parms;

	UObject::ProcessEvent(Func, &Parms);

	return Parms.ReturnValue;
};
struct UClass* UGameplayStatics::StaticClass()
{
	return (UClass*)ObjObjects->FindObject("Class Engine.GameplayStatics");
}
struct UClass* UConsole::StaticClass()
{
	return (UClass*)ObjObjects->FindObject("Class Engine.Console");
}
struct UClass* UKismetStringLibrary::StaticClass()
{
	return (UClass*)ObjObjects->FindObject("Class Engine.KismetStringLibrary");
}
struct UClass* UPortalWarsNotificationManager::StaticClass()
{
	return (UClass*)ObjObjects->FindObject("Class PortalWars.PortalWarsNotificationManager");
}
struct UPortalWarsNotificationManager* UPortalWarsNotificationManager::GetDefaultObj()
{
	static struct UPortalWarsNotificationManager* Default = nullptr;

	if (!Default)
		Default = static_cast<UPortalWarsNotificationManager*>((UObject*)UPortalWarsNotificationManager::StaticClass());

	return Default;
}
struct UClass* UKismetTextLibrary::StaticClass()
{
	return (UClass*)ObjObjects->FindObject("Class Engine.KismetTextLibrary");
}
USkeletalMeshComponent* ACharacterSkin::GetMesh3P()
{
	static UObject* Function = nullptr;
	if (!Function) Function = ObjObjects->FindObject("Function PortalWars.BaseCharacterSkin.GetMesh3P");
	if (!Function) Function = ObjObjects->FindObject("Function PortalWars.CharacterSkin.GetMesh3P");
	if (!Function) return nullptr;

	struct
	{
		USkeletalMeshComponent* ReturnValue;
	} Parameters{};

	ProcessEvent(Function, &Parameters);
	return Parameters.ReturnValue;
}
AActor* UGameplayStatics::BeginDeferredActorSpawnFromClass(UObject* WorldContextObject, UClass* ActorClass, FTransform SpawnTransform, ESpawnActorCollisionHandlingMethod CollisionHandlingOverride, AActor* Owner)
{
	static auto Function = ObjObjects->FindObject("Function Engine.GameplayStatics.BeginDeferredActorSpawnFromClass");
	if (!Function) return nullptr;

	// Layout matches UE's param struct: FTransform (16-aligned) lands at 0x10 after the two
	// pointers, the collision byte at 0x40, Owner at 0x48, ReturnValue at 0x50.
	struct
	{
		UObject* WorldContextObject;						 // 0x00
		UClass* ActorClass;									 // 0x08
		FTransform SpawnTransform;							 // 0x10
		ESpawnActorCollisionHandlingMethod CollisionHandling; // 0x40
		AActor* Owner;										 // 0x48
		AActor* ReturnValue;								 // 0x50
	} Parameters;
	Parameters.WorldContextObject = WorldContextObject;
	Parameters.ActorClass = ActorClass;
	Parameters.SpawnTransform = SpawnTransform;
	Parameters.CollisionHandling = CollisionHandlingOverride;
	Parameters.Owner = Owner;

	ProcessEvent(Function, &Parameters);

	return Parameters.ReturnValue;
}
AActor* UGameplayStatics::FinishSpawningActor(AActor* Actor, FTransform SpawnTransform)
{
	static auto Function = ObjObjects->FindObject("Function Engine.GameplayStatics.FinishSpawningActor");
	if (!Function) return nullptr;

	// UE 16-aligns FTransform, so SpawnTransform sits at 0x10 (pad after Actor), ReturnValue at 0x40.
	struct
	{
		AActor* Actor;			   // 0x00
		char pad_08[0x8];		   // 0x08
		FTransform SpawnTransform; // 0x10
		AActor* ReturnValue;	   // 0x40
	} Parameters;
	Parameters.Actor = Actor;
	Parameters.SpawnTransform = SpawnTransform;

	ProcessEvent(Function, &Parameters);

	return Parameters.ReturnValue;
}
AActor* SpawnActor(UObject* worldContextObject, UClass* actorClass, FVector location, ESpawnActorCollisionHandlingMethod collision, AActor* owner)
{
	auto* gameplayStatics = reinterpret_cast<UGameplayStatics*>(UGameplayStatics::StaticClass());
	if (!gameplayStatics || !actorClass) return nullptr;

	FTransform transform{};
	transform.Rotation = FQuat{0.f, 0.f, 0.f, 1.f};
	transform.Translation = location;
	transform.Scale3D = FVector{1.f, 1.f, 1.f};

	AActor* deferred = gameplayStatics->BeginDeferredActorSpawnFromClass(worldContextObject, actorClass, transform, collision, owner);
	if (!deferred) return nullptr;

	return gameplayStatics->FinishSpawningActor(deferred, transform);
}
bool IsPostGameController(UObject* controller)
{
	if (!controller) return false;
	static UObject* postClass = nullptr;
	if (!postClass) postClass = ObjObjects->FindObject("Class PortalWars.PortalWarsPostPlayerController");
	return postClass && controller->IsA(postClass);
}
bool LineTraceVisible(UObject* worldContext, const FVector& start, const FVector& end, AActor* ignoreActor)
{
	static auto Function = ObjObjects->FindObject("Function Engine.KismetSystemLibrary.LineTraceSingle");
	static auto* library = reinterpret_cast<UObject*>(ObjObjects->FindObject("Class Engine.KismetSystemLibrary"));
	if (!Function || !library || !worldContext) return true; // fail-open: never block callers if the probe can't run

	// ActorsToIgnore is a real TArray<AActor*>; back it with a stack slot the trace only reads.
	AActor* ignoreArr[1] = {ignoreActor};

	// UKismetSystemLibrary::LineTraceSingle parameter block. The offsets are the engine's (verified
	// by the static_asserts below); every member already lands on its natural alignment, so no
	// packing pragma is needed. FHitResult (0x88) sits 8-aligned at 0x40.
	struct Params
	{
		UObject* WorldContextObject; // 0x00
		FVector Start;				 // 0x08
		FVector End;				 // 0x14
		uint8_t TraceChannel;		 // 0x20  ETraceTypeQuery (0 = Visibility)
		bool bTraceComplex;			 // 0x21
		char pad_22[6];				 // 0x22
		struct
		{
			void* Data;
			int32_t Num;
			int32_t Max;
		} ActorsToIgnore;	   // 0x28  TArray<AActor*>
		uint8_t DrawDebugType; // 0x38  EDrawDebugTrace (0 = None)
		char pad_39[7];		   // 0x39
		FHitResult OutHit;	   // 0x40
		bool bIgnoreSelf;	   // 0xC8
		char pad_C9[3];		   // 0xC9
		FLinearColor TraceColor;	// 0xCC
		FLinearColor TraceHitColor; // 0xDC
		float DrawTime;				// 0xEC
		bool ReturnValue;			// 0xF0
	} p{};

	static_assert(offsetof(Params, ActorsToIgnore) == 0x28, "ActorsToIgnore offset");
	static_assert(offsetof(Params, DrawDebugType) == 0x38, "DrawDebugType offset");
	static_assert(offsetof(Params, OutHit) == 0x40, "OutHit offset");
	static_assert(offsetof(Params, bIgnoreSelf) == 0xC8, "bIgnoreSelf offset");
	static_assert(offsetof(Params, ReturnValue) == 0xF0, "ReturnValue offset");

	p.WorldContextObject = worldContext;
	p.Start = start;
	p.End = end;
	p.TraceChannel = 0; // Visibility
	p.bTraceComplex = false;
	p.ActorsToIgnore.Data = ignoreActor ? ignoreArr : nullptr;
	p.ActorsToIgnore.Num = ignoreActor ? 1 : 0;
	p.ActorsToIgnore.Max = ignoreActor ? 1 : 0;
	p.DrawDebugType = 0; // None
	p.bIgnoreSelf = true;

	library->ProcessEvent(Function, &p);

	// ReturnValue is bBlockingHit: with the target ignored, a blocking hit means something (a wall)
	// stands between start and end, i.e. the point is occluded.
	return !p.ReturnValue;
}
struct UClass* ACharacter::StaticClass()
{
	return (UClass*)ObjObjects->FindObject("Class Engine.Character");
};
struct UClass* APawn::StaticClass()
{
	return (UClass*)ObjObjects->FindObject("Class Engine.Pawn");
};
struct UClass* APortalWarsCharacter::StaticClass()
{
	return (UClass*)ObjObjects->FindObject("Class PortalWars.PortalWarsCharacter");
};
struct UClass* ACullableActor::StaticClass()
{
	return (UClass*)ObjObjects->FindObject("Class PortalWars.CullableActor");
};
FVector USkeletalMeshComponent::GetBoneMatrix(int index)
{

	auto GetBoneMatrix = reinterpret_cast<FMatrix* (*)(USkeletalMeshComponent*, FMatrix*, int)>(GetBoneMatrixF);

	FMatrix matrix;
	GetBoneMatrix(this, &matrix, index);

	return FVector({matrix.M[3][0], matrix.M[3][1], matrix.M[3][2]});
}
FVector2D USkeletalMeshComponent::GetBone(int index, APlayerController* PlayerController)
{

	FVector WorldLocation = this->GetBoneMatrix(index);

	FVector2D ScreenLocation;

	if (PlayerController->ProjectWorldLocationToScreen(WorldLocation, ScreenLocation, false)) return ScreenLocation;

	return {0, 0};
}
