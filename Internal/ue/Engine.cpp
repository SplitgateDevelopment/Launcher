/// @file
/// @brief Out-of-line implementations for the reverse-engineered UE SDK.
///
/// Provides the definitions for the SDK types declared in Engine.h that cannot
/// be header-only (FName/name-pool resolution, UObject name/full-name lookup,
/// object and function finding, etc.). Companion to the generated Engine.h.
#include "Engine.h"
#include "../memory/Memory.h"
#include <Psapi.h>
#include <cstddef>
#include "UObjects.h"

// The engine globals/UFunctions now live in namespace Engine; pull them into scope
// so the out-of-line SDK bodies below keep referencing them unqualified.
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

FNamePool* Engine::NamePoolData = nullptr;
TUObjectArray* Engine::ObjObjects = nullptr;
UWorld* Engine::WRLD = nullptr;

uintptr_t Engine::GetBoneMatrixF;

void APlayerController::SwitchLevel(FString URL)
{
	struct
	{
		FString URL;
	} Parameters;
	Parameters.URL = URL;

	ProcessEvent(UObjects::SwitchLevelUFunc, &Parameters);
}

void APlayerController::FOV(float NewFOV)
{
	struct
	{
		float NewFOV;
	} Parameters;
	Parameters.NewFOV = NewFOV;

	ProcessEvent(UObjects::FOVUFunc, &Parameters);
}

void APlayerController::SetName(FString S)
{
	struct
	{
		FString S;
	} Parameters;
	Parameters.S = S;

	ProcessEvent(UObjects::SetNameUFunc, &Parameters);
}

void UCanvas::K2_DrawLine(FVector2D ScreenPositionA, FVector2D ScreenPositionB, FLOAT Thickness, FLinearColor Color)
{
	struct
	{
		FVector2D ScreenPositionA;
		FVector2D ScreenPositionB;
		FLOAT Thickness;
		FLinearColor Color;
	} Parameters;

	Parameters.ScreenPositionA = ScreenPositionA;
	Parameters.ScreenPositionB = ScreenPositionB;
	Parameters.Thickness = Thickness;
	Parameters.Color = Color;

	ProcessEvent(UObjects::K2_DrawLineUFunc, &Parameters);
}

void UCanvas::K2_DrawText(struct UFont* RenderFont, struct FString RenderText, struct FVector2D ScreenPosition, struct FVector2D Scale, struct FLinearColor RenderColor, float Kerning, struct FLinearColor ShadowColor, struct FVector2D ShadowOffset, bool bCentreX, bool bCentreY, bool bOutlined, struct FLinearColor OutlineColor)
{
	struct
	{
		struct UFont* RenderFont;
		FString RenderText;
		FVector2D ScreenPosition;
		FVector2D Scale;
		FLinearColor RenderColor;
		float Kerning;
		FLinearColor ShadowColor;
		FVector2D ShadowOffset;
		bool bCentreX;
		bool bCentreY;
		bool bOutlined;
		FLinearColor OutlineColor;
	} Parameters;

	Parameters.RenderFont = RenderFont ? RenderFont : (UFont*)UObjects::Font;
	Parameters.RenderText = RenderText;
	Parameters.ScreenPosition = ScreenPosition;
	Parameters.Scale = Scale;
	Parameters.RenderColor = RenderColor;
	Parameters.Kerning = Kerning;
	Parameters.ShadowColor = ShadowColor;
	Parameters.ShadowOffset = ShadowOffset;
	Parameters.bCentreX = bCentreX;
	Parameters.bCentreY = bCentreY;
	Parameters.bOutlined = bOutlined;

	ProcessEvent(UObjects::K2_DrawTextUFunc, &Parameters);
};

UPortalWarsSaveGame* UPortalWarsLocalPlayer::GetUserSaveGame()
{
	struct
	{
		UPortalWarsSaveGame* ReturnValue;
	} Parameters;

	ProcessEvent(UObjects::GetSaveGameUFUnc, &Parameters);
	return Parameters.ReturnValue;
};

void USceneComponent::K2_SetRelativeRotation(struct FRotator NewRotation, bool bSweep, struct FHitResult& SweepHitResult, bool bTeleport)
{
	struct
	{
		FRotator NewRotation;
		bool bSweep;
		FHitResult SweepHitResult;
		bool bTeleport;
	} Parameters;
	Parameters.NewRotation = NewRotation;
	Parameters.bSweep = bSweep;
	Parameters.SweepHitResult = SweepHitResult;
	Parameters.bTeleport = bTeleport;

	ProcessEvent(UObjects::K2_SetRelativeRotationUFunc, &Parameters);
}

bool AActor::K2_TeleportTo(struct FVector DestLocation, struct FRotator DestRotation)
{
	struct
	{
		FVector DestLocation;
		FRotator DestRotation;
		bool ReturnValue;
	} Parameters;
	Parameters.DestLocation = DestLocation;
	Parameters.DestRotation = DestRotation;

	ProcessEvent(UObjects::K2_TeleportTo, &Parameters);
	return Parameters.ReturnValue;
};

FVector AActor::K2_GetActorLocation()
{
	struct
	{
		FVector ReturnValue;
	} Parameters;

	ProcessEvent(UObjects::K2_GetActorLocation, &Parameters);
	return Parameters.ReturnValue;
};

FRotator AActor::K2_GetActorRotation()
{
	struct
	{
		FRotator ReturnValue;
	} Parameters;

	ProcessEvent(UObjects::K2_GetActorRotation, &Parameters);
	return Parameters.ReturnValue;
};

void AActor::SetActorEnableCollision(bool bNewActorEnableCollision)
{
	struct
	{
		bool bNewActorEnableCollision;
	} Parameters;
	Parameters.bNewActorEnableCollision = bNewActorEnableCollision;

	ProcessEvent(UObjects::SetActorEnableCollision, &Parameters);
};

bool AActor::GetActorEnableCollision()
{
	struct
	{
		bool ReturnValue;
	} Parameters;

	ProcessEvent(UObjects::GetActorEnableCollision, &Parameters);
	return Parameters.ReturnValue;
};

bool AActor::K2_SetActorLocation(struct FVector NewLocation, bool bSweep, struct FHitResult& SweepHitResult, bool bTeleport)
{
	struct
	{
		FVector NewLocation;
		bool bSweep;
		FHitResult SweepHitResult;
		bool bTeleport;
		bool ReturnValue;
	} Parameters;

	Parameters.NewLocation = NewLocation;
	Parameters.bSweep = bSweep;
	Parameters.SweepHitResult = SweepHitResult;
	Parameters.bTeleport = bTeleport;

	ProcessEvent(UObjects::K2_SetActorLocation, &Parameters);
	return Parameters.ReturnValue;
}

struct UObject* UGameplayStatics::SpawnObject(struct UObject* ObjectClass, struct UObject* Outer)
{
	UObject* SpawnObject = ObjObjects->FindObject("Function Engine.GameplayStatics.SpawnObject");

	struct
	{
		UObject* ObjectClass;
		UObject* Outer;
		UObject* ReturnValue;
	} Parameters;

	Parameters.ObjectClass = ObjectClass;
	Parameters.Outer = Outer;

	ProcessEvent(SpawnObject, &Parameters);

	return Parameters.ReturnValue;
}

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

struct FName UKismetStringLibrary::Conv_StringToName(struct FString InString)
{
	auto Func = ObjObjects->FindObject("Function Engine.KismetStringLibrary.Conv_StringToName");

	struct
	{
		struct FString InString;
		struct FName ReturnValue;
	} Parms;

	Parms.InString = InString;

	ProcessEvent(Func, &Parms);

	return Parms.ReturnValue;
};

void APlayerController::ConsoleKey(struct FKey Key)
{
	auto Func = ObjObjects->FindObject("Function Engine.PlayerController.ConsoleKey");

	struct
	{
		struct FKey Key;
	} Parms;

	Parms.Key = Key;

	ProcessEvent(Func, &Parms);
};

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

void UPortalWarsNotificationManager::OpenNoticeDialog(FErrorInfo& ErrorInfo)
{
	auto Function = ObjObjects->FindObject("Function PortalWars.PortalWarsNotificationManager.OpenNoticeDialog");

	struct
	{
		FErrorInfo ErrorInfo;
	} Params;
	Params.ErrorInfo = ErrorInfo;

	UObject::ProcessEvent(Function, &Params);
}

void UPortalWarsNotificationManager::OpenErrorDialog(FErrorInfo& ErrorInfo)
{
	auto Function = ObjObjects->FindObject("Function PortalWars.PortalWarsNotificationManager.OpenErrorDialog");

	struct
	{
		FErrorInfo ErrorInfo;
	} Params;
	Params.ErrorInfo = ErrorInfo;

	UObject::ProcessEvent(Function, &Params);
}

void APortalWarsPlayerController::ClientUpdateChat(struct FTextChatData InData)
{
	auto Function = ObjObjects->FindObject("Function PortalWars.PortalWarsPlayerController.ClientUpdateChat");

	struct
	{
		struct FTextChatData InData;
	} Params;
	Params.InData = InData;

	UObject::ProcessEvent(Function, &Params);
}

struct UClass* UKismetTextLibrary::StaticClass()
{
	return (UClass*)ObjObjects->FindObject("Class Engine.KismetTextLibrary");
}

struct FText UKismetTextLibrary::Conv_StringToText(struct FString InString)
{
	auto Func = ObjObjects->FindObject("Function Engine.KismetStringLibrary.Conv_StringToText");

	struct
	{
		struct FString InString;
		struct FText ReturnValue;
	} Parms;

	Parms.InString = InString;

	ProcessEvent(Func, &Parms);

	return Parms.ReturnValue;
};

void APlayerController::ClientSetCameraMode(struct FName NewCameraMode)
{
	auto Function = ObjObjects->FindObject("Function Engine.PlayerController.ClientSetCameraMode");

	struct
	{
		struct FName NewCameraMode;
	} Params;
	Params.NewCameraMode = NewCameraMode;

	UObject::ProcessEvent(Function, &Params);
}

void APlayerController::SendToConsole(FString Command)
{
	auto Function = ObjObjects->FindObject("Function Engine.PlayerController.SendToConsole");

	struct
	{
		FString Command;
	} Params;
	Params.Command = Command;

	UObject::ProcessEvent(Function, &Params);
}

bool APlayerController::IsInGame()
{
	// A pawn is acknowledged AND we're not in the post-match lobby controller. Excluding the
	// post-game controller here makes every feature + the actor cache stand down after a match
	// (they all gate on IsInGame), which is where the post-game FPS drop came from.
	return this->AcknowledgedPawn && !IsPostGameController(reinterpret_cast<UObject*>(this));
}

void APortalWarsPlayerController::SendChatMessage(FString Message, enum class EChatType ChatType)
{
	if (!this->IsInGame()) return;

	FString sender = FString("[Splitgate Internal]");
	FTextChatData ChatData{};

	ChatData.SenderName = sender;
	ChatData.SenderText = Message;
	ChatData.NiceText = Message;
	ChatData.ChatType = ChatType;
	ChatData.SenderID = {};

	this->ClientUpdateChat(ChatData);
}

void AActor::GetActorBounds(bool bOnlyCollidingComponents, struct FVector& Origin, struct FVector& BoxExtent, bool bIncludeFromChildActors)
{
	auto Function = ObjObjects->FindObject("Function Engine.Actor.GetActorBounds");

	struct
	{
		bool bOnlyCollidingComponents;
		struct FVector Origin;
		struct FVector BoxExtent;
		bool bIncludeFromChildActors;
	} Params;
	Params.bOnlyCollidingComponents = bOnlyCollidingComponents;
	Params.Origin = Origin;
	Params.BoxExtent = BoxExtent;
	Params.bIncludeFromChildActors = bIncludeFromChildActors;

	UObject::ProcessEvent(Function, &Params);
};

bool APlayerController::ProjectWorldLocationToScreen(FVector WorldLocation, FVector2D& ScreenLocation, bool bPlayerViewportRelative)
{
	// Resolve the UFunction once and reuse it. This is on the ESP hot path (one call per bone per
	// enemy per frame); doing the object-array lookup every call was the main ESP frame-rate cost.
	static auto Function = ObjObjects->FindObject("Function Engine.PlayerController.ProjectWorldLocationToScreen");

	struct
	{
		FVector WorldLocation;
		FVector2D ScreenLocation;
		bool bPlayerViewportRelative;
		bool ReturnValue;
	} Parameters;

	Parameters.WorldLocation = WorldLocation;
	Parameters.ScreenLocation = ScreenLocation;
	Parameters.bPlayerViewportRelative = bPlayerViewportRelative;

	ProcessEvent(Function, &Parameters);

	ScreenLocation = Parameters.ScreenLocation;

	return Parameters.ReturnValue;
};

FVector APlayerCameraManager::GetCameraLocation()
{
	static auto Function = ObjObjects->FindObject("Function Engine.PlayerCameraManager.GetCameraLocation");

	struct
	{
		FVector ReturnValue;
	} Parameters;

	ProcessEvent(Function, &Parameters);

	return Parameters.ReturnValue;
};

FRotator APlayerCameraManager::GetCameraRotation()
{
	static auto Function = ObjObjects->FindObject("Function Engine.PlayerCameraManager.GetCameraRotation");

	struct
	{
		FRotator ReturnValue;
	} Parameters;

	ProcessEvent(Function, &Parameters);

	return Parameters.ReturnValue;
};

float APlayerCameraManager::GetFOVAngle()
{
	static auto Function = ObjObjects->FindObject("Function Engine.PlayerCameraManager.GetFOVAngle");

	struct
	{
		float ReturnValue;
	} Parameters;

	ProcessEvent(Function, &Parameters);

	return Parameters.ReturnValue;
};

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

void ACharacter::LaunchCharacter(FVector LaunchVelocity, bool bXYOverride, bool bZOverride)
{
	static auto Function = ObjObjects->FindObject("Function Engine.Character.LaunchCharacter");
	if (!Function) return;

	struct
	{
		FVector LaunchVelocity;
		bool bXYOverride;
		bool bZOverride;
	} Parameters;
	Parameters.LaunchVelocity = LaunchVelocity;
	Parameters.bXYOverride = bXYOverride;
	Parameters.bZOverride = bZOverride;

	ProcessEvent(Function, &Parameters);
}

bool APortalWarsPlayerController::ProjectWorldLocationToScreenCustom(FVector WorldLocation, FVector2D& ScreenLocation, bool bPlayerViewportRelative)
{
	static auto Function = ObjObjects->FindObject("Function PortalWars.PortalWarsPlayerController.ProjectWorldLocationToScreenCustom");
	if (!Function) return false;

	struct
	{
		FVector WorldLocation;			 // 0x00
		FVector2D ScreenLocation;		 // 0x0c (out)
		bool bPlayerViewportRelative;	 // 0x14
		bool ReturnValue;				 // 0x15
	} Parameters;
	Parameters.WorldLocation = WorldLocation;
	Parameters.ScreenLocation = {};
	Parameters.bPlayerViewportRelative = bPlayerViewportRelative;

	ProcessEvent(Function, &Parameters);

	ScreenLocation = Parameters.ScreenLocation;
	return Parameters.ReturnValue;
}

void APortalWarsPlayerController::ClientSetSpectatorCamera(FVector CameraLocation, FRotator CameraRotation)
{
	static auto Function = ObjObjects->FindObject("Function PortalWars.PortalWarsPlayerController.ClientSetSpectatorCamera");
	if (!Function) return;

	struct
	{
		FVector CameraLocation;
		FRotator CameraRotation;
	} Parameters;
	Parameters.CameraLocation = CameraLocation;
	Parameters.CameraRotation = CameraRotation;

	ProcessEvent(Function, &Parameters);
}

void APortalWarsCharacter::UpdateSkins()
{
	static auto Function = ObjObjects->FindObject("Function PortalWars.PortalWarsCharacter.UpdateSkins");
	if (!Function) return;

	ProcessEvent(Function, nullptr);
}

void ABaseGun::UpdateSkins()
{
	static auto Function = ObjObjects->FindObject("Function PortalWars.BaseGun.UpdateSkins");
	if (!Function) return;

	ProcessEvent(Function, nullptr);
}

void APortalWarsCharacter::RequestSuicide()
{
	static auto Function = ObjObjects->FindObject("Function PortalWars.PortalWarsCharacter.RequestSuicide");
	if (!Function) return;

	ProcessEvent(Function, nullptr);
}

char APortalWarsCharacter::GetTeamNum()
{
	static auto Function = ObjObjects->FindObject("Function PortalWars.PortalWarsCharacter.GetTeamNum");

	struct
	{
		char ReturnValue;
	} Parameters;

	ProcessEvent(Function, &Parameters);

	return Parameters.ReturnValue;
}

bool AActor::WasRecentlyRendered(float Tolerance)
{
	static auto Function = ObjObjects->FindObject("Function Engine.Actor.WasRecentlyRendered");
	if (!Function) return true; // if the check can't run, don't hide the actor

	struct
	{
		float Tolerance;
		bool ReturnValue;
	} Parameters;
	Parameters.Tolerance = Tolerance;

	ProcessEvent(Function, &Parameters);

	return Parameters.ReturnValue;
}

FVector AActor::GetVelocity()
{
	static auto Function = ObjObjects->FindObject("Function Engine.Actor.GetVelocity");
	if (!Function) return FVector{0.f, 0.f, 0.f};

	struct
	{
		FVector ReturnValue;
	} Parameters;

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

void UPrimitiveComponent::SetRenderCustomDepth(bool bValue)
{
	static auto Function = ObjObjects->FindObject("Function Engine.PrimitiveComponent.SetRenderCustomDepth");
	if (!Function) return;

	struct
	{
		bool bValue;
	} Parameters;
	Parameters.bValue = bValue;

	ProcessEvent(Function, &Parameters);
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

void APlayerController::ClientMessage(FString S, FName Type, float MsgLifeTime)
{
	static auto Function = ObjObjects->FindObject("Function Engine.PlayerController.ClientMessage");
	if (!Function) return;

	struct
	{
		FString S;
		FName Type;
		float MsgLifeTime;
	} Parameters;
	Parameters.S = S;
	Parameters.Type = Type;
	Parameters.MsgLifeTime = MsgLifeTime;

	ProcessEvent(Function, &Parameters);
}

void AController::SetControlRotation(FRotator& NewRotation)
{
	static auto Function = ObjObjects->FindObject("Function Engine.Controller.SetControlRotation");
	if (!Function) return;

	struct
	{
		FRotator NewRotation;
	} Parameters;
	Parameters.NewRotation = NewRotation;

	ProcessEvent(Function, &Parameters);
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

struct FName USkinnedMeshComponent::GetBoneName(int32_t BoneIndex)
{
	static auto Function = ObjObjects->FindObject("Function Engine.SkinnedMeshComponent.GetBoneName");
	struct
	{
		int32_t BoneIndex;
		FName ReturnValue;
	} Parameters;

	Parameters.BoneIndex = BoneIndex;

	ProcessEvent(Function, &Parameters);

	return Parameters.ReturnValue;
}

bool Engine::Init()
{
	auto main = GetModuleHandleA(nullptr);

	static byte objSig[] = {0x48, 0x8B, 0x05, 0x00, 0x00, 0x00, 0x00, 0x48, 0x8B, 0x0C, 0xC8, 0x48, 0x8D, 0x04, 0xD1, 0xEB};
	ObjObjects = reinterpret_cast<decltype(ObjObjects)>(Memory::FindPointer(main, objSig, sizeof(objSig), 0));
	if (!ObjObjects) return false;

	static byte poolSig[] = {0x48, 0x8D, 0x0D, 0x00, 0x00, 0x00, 0x00, 0xE8, 0x00, 0x00, 0x00, 0x00, 0xC6, 0x05, 0x00, 0x00, 0x00, 0x00, 0x01, 0x0F, 0x10, 0x03, 0x4C, 0x8D, 0x44, 0x24, 0x20, 0x48, 0x8B, 0xC8};
	NamePoolData = reinterpret_cast<decltype(NamePoolData)>(Memory::FindPointer(main, poolSig, sizeof(poolSig), 0));
	if (!NamePoolData) return false;

	static byte worldSig[] = {0x48, 0x8B, 0x1D, 0x00, 0x00, 0x00, 0x00, 0x48, 0x85, 0xDB, 0x74, 0x3B, 0x41, 0xB0, 0x01, 0x33, 0xD2, 0x48, 0x8B, 0xCB, 0xE8};
	WRLD = reinterpret_cast<decltype(WRLD)>(Memory::FindPointer(main, worldSig, sizeof(worldSig), 0));
	if (!WRLD) return false;

	static byte GetBoneMatrixSig[] = {0x48, 0x8B, 0xC4, 0x55, 0x53, 0x56, 0x57, 0x41, 0x54, 0x41, 0x56, 0x41, 0x57, 0x48, 0x8D, 0x68, 0xA1, 0x48, 0x81, 0xEC, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x29, 0x78, 0xB8, 0x33, 0xF6, 0x44, 0x0F, 0x29, 0x40};
	MODULEINFO info;
	if (K32GetModuleInformation(GetCurrentProcess(), main, &info, sizeof(MODULEINFO)))
	{
		auto base = static_cast<byte*>(info.lpBaseOfDll);
		GetBoneMatrixF = reinterpret_cast<decltype(GetBoneMatrixF)>(Memory::Find(base, base + info.SizeOfImage - 1, Memory::FromBytes(GetBoneMatrixSig, sizeof(GetBoneMatrixSig))));
		if (!GetBoneMatrixF) return false;
	}

	UObjects::Init();
	return true;
}
