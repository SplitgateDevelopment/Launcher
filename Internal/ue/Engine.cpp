#include "Engine.h"
#include "../utils/Util.h"
#include <Psapi.h>
#include "UObjects.h"

std::string FNameEntry::String()
{
	if (bIsWide) { return std::string(); }
	return { AnsiName, Len };
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
	for (auto outer = OuterPrivate; outer; outer = outer->OuterPrivate) { name = outer->GetName() + "." + name; }
	name = ClassPrivate->GetName() + " " + name + this->GetName();
	return name;
}

bool UObject::IsA(void* cmp)
{
	for (auto super = ClassPrivate; super; super = static_cast<UClass*>(super->SuperStruct)) { if (super == cmp) { return true; } }
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
		if (object && object->GetFullName() == name) { return object; }
	}
	return nullptr;
}

void UObject::ProcessEvent(void* UFunction, void* Params)
{
	auto vtable = *reinterpret_cast<void***>(this);
	reinterpret_cast<void(*)(void*, void*, void*)>(vtable[68])(this, UFunction, Params);
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

FNamePool* NamePoolData = nullptr;
TUObjectArray* ObjObjects = nullptr;
UWorld* WRLD = nullptr;

uintptr_t GetBoneMatrixF;

void APlayerController::SwitchLevel(FString URL)
{
	struct {
		FString URL;
	} Parameters;
	Parameters.URL = URL;

	ProcessEvent(UObjects::SwitchLevelUFunc, &Parameters);
}

void APlayerController::FOV(float NewFOV)
{
	struct {
		float NewFOV;
	} Parameters;
	Parameters.NewFOV = NewFOV;

	ProcessEvent(UObjects::FOVUFunc, &Parameters);
}

void APlayerController::SetName(FString S)
{
	struct {
		FString S;
	} Parameters;
	Parameters.S = S;

	ProcessEvent(UObjects::SetNameUFunc, &Parameters);
}

void UCanvas::K2_DrawLine(FVector2D ScreenPositionA, FVector2D ScreenPositionB, FLOAT Thickness, FLinearColor Color)
{
	struct {
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
	struct {
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

UPortalWarsSaveGame* UPortalWarsLocalPlayer::GetUserSaveGame() {
	struct {
		UPortalWarsSaveGame* ReturnValue;
	} Parameters;

	ProcessEvent(UObjects::GetSaveGameUFUnc, &Parameters);
	return Parameters.ReturnValue;
};

void USceneComponent::K2_SetRelativeRotation(struct FRotator NewRotation, bool bSweep, struct FHitResult& SweepHitResult, bool bTeleport)
{
	struct {
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

bool AActor::K2_TeleportTo(struct FVector DestLocation, struct FRotator DestRotation) {
	struct {
		FVector DestLocation;
		FRotator DestRotation;
		bool ReturnValue;
	} Parameters;
	Parameters.DestLocation = DestLocation;
	Parameters.DestRotation = DestRotation;

	ProcessEvent(UObjects::K2_TeleportTo, &Parameters);
	return Parameters.ReturnValue;
};

FVector AActor::K2_GetActorLocation() {
	struct {
		FVector ReturnValue;
	} Parameters;

	ProcessEvent(UObjects::K2_GetActorLocation, &Parameters);
	return Parameters.ReturnValue;
};

FRotator AActor::K2_GetActorRotation() {
	struct {
		FRotator ReturnValue;
	} Parameters;

	ProcessEvent(UObjects::K2_GetActorRotation, &Parameters);
	return Parameters.ReturnValue;
};

void AActor::SetActorEnableCollision(bool bNewActorEnableCollision) {
	struct {
		bool bNewActorEnableCollision;
	} Parameters;
	Parameters.bNewActorEnableCollision = bNewActorEnableCollision;

	ProcessEvent(UObjects::SetActorEnableCollision, &Parameters);
};

bool AActor::GetActorEnableCollision() {
	struct {
		bool ReturnValue;
	} Parameters;

	ProcessEvent(UObjects::GetActorEnableCollision, &Parameters);
	return Parameters.ReturnValue;
};

bool AActor::K2_SetActorLocation(struct FVector NewLocation, bool bSweep, struct FHitResult& SweepHitResult, bool bTeleport)
{
	struct {
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

	struct {
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

	struct {
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

	struct {
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

	struct {
		FErrorInfo ErrorInfo;
	} Params;
	Params.ErrorInfo = ErrorInfo;

	UObject::ProcessEvent(Function, &Params);
}

void UPortalWarsNotificationManager::OpenErrorDialog(FErrorInfo& ErrorInfo)
{
	auto Function = ObjObjects->FindObject("Function PortalWars.PortalWarsNotificationManager.OpenErrorDialog");

	struct {
		FErrorInfo ErrorInfo;
	} Params;
	Params.ErrorInfo = ErrorInfo;

	UObject::ProcessEvent(Function, &Params);
}

void APortalWarsPlayerController::ClientUpdateChat(struct FTextChatData InData)
{
	auto Function = ObjObjects->FindObject("Function PortalWars.PortalWarsPlayerController.ClientUpdateChat");

	struct {
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

	struct {
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

	struct {
		struct FName NewCameraMode;
	} Params;
	Params.NewCameraMode = NewCameraMode;

	UObject::ProcessEvent(Function, &Params);
}

void APlayerController::SendToConsole(FString Command)
{
	auto Function = ObjObjects->FindObject("Function Engine.PlayerController.ClientSetCameraMode");

	struct {
		FString Command;
	} Params;
	Params.Command = Command;

	UObject::ProcessEvent(Function, &Params);
}

bool APlayerController::IsInGame()
{
	return (this->AcknowledgedPawn);
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

	struct {
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
	auto Function = ObjObjects->FindObject("Function Engine.PlayerController.ProjectWorldLocationToScreen");

	struct {
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

FVector USkeletalMeshComponent::GetBoneMatrix(int index) {

	auto GetBoneMatrix = reinterpret_cast<FMatrix * (*)(USkeletalMeshComponent*, FMatrix*, int)>(GetBoneMatrixF);

	FMatrix matrix;
	GetBoneMatrix(this, &matrix, index);

	return FVector({ matrix.M[3][0], matrix.M[3][1], matrix.M[3][2] });
}

FVector2D USkeletalMeshComponent::GetBone(int index, APlayerController* PlayerController) {

	FVector WorldLocation = this->GetBoneMatrix(index);

	FVector2D ScreenLocation;

	if (PlayerController->ProjectWorldLocationToScreen(WorldLocation, ScreenLocation, false)) return ScreenLocation;

	return { 0,0 };
}

struct FName USkinnedMeshComponent::GetBoneName(int32_t BoneIndex)
{
	static auto Function = ObjObjects->FindObject("Function Engine.SkinnedMeshComponent.GetBoneName");
	struct {
		int32_t BoneIndex;
		FName ReturnValue;
	} Parameters;

	Parameters.BoneIndex = BoneIndex;

	ProcessEvent(Function, &Parameters);

	return Parameters.ReturnValue;
}

bool EngineInit()
{
	auto main = GetModuleHandleA(nullptr);

	static byte objSig[] = { 0x48, 0x8B, 0x05, 0x00, 0x00, 0x00, 0x00, 0x48, 0x8B, 0x0C, 0xC8, 0x48, 0x8D, 0x04, 0xD1, 0xEB };
	ObjObjects = reinterpret_cast<decltype(ObjObjects)>(FindPointer(main, objSig, sizeof(objSig), 0));
	if (!ObjObjects) return false;

	static byte poolSig[] = { 0x48, 0x8D, 0x0D, 0x00, 0x00, 0x00, 0x00, 0xE8, 0x00, 0x00, 0x00, 0x00, 0xC6, 0x05, 0x00, 0x00, 0x00, 0x00, 0x01, 0x0F, 0x10, 0x03, 0x4C, 0x8D, 0x44, 0x24, 0x20, 0x48, 0x8B, 0xC8 };
	NamePoolData = reinterpret_cast<decltype(NamePoolData)>(FindPointer(main, poolSig, sizeof(poolSig), 0));
	if (!NamePoolData) return false;

	static byte worldSig[] = { 0x48, 0x8B, 0x1D, 0x00, 0x00, 0x00, 0x00, 0x48, 0x85, 0xDB, 0x74, 0x3B, 0x41, 0xB0, 0x01, 0x33, 0xD2, 0x48, 0x8B, 0xCB, 0xE8 };
	WRLD = reinterpret_cast<decltype(WRLD)>(FindPointer(main, worldSig, sizeof(worldSig), 0));
	if (!WRLD) return false;

	static byte GetBoneMatrixSig[] = { 0x48, 0x8B, 0xC4, 0x55, 0x53, 0x56, 0x57, 0x41, 0x54, 0x41, 0x56, 0x41, 0x57, 0x48, 0x8D, 0x68, 0xA1, 0x48, 0x81, 0xEC, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x29, 0x78, 0xB8, 0x33, 0xF6, 0x44, 0x0F, 0x29, 0x40 };
	MODULEINFO info;
	if (K32GetModuleInformation(GetCurrentProcess(), main, &info, sizeof(MODULEINFO))) {
		auto base = static_cast<byte*>(info.lpBaseOfDll);
		GetBoneMatrixF = reinterpret_cast<decltype(GetBoneMatrixF)>(FindSignature(base, base + info.SizeOfImage - 1, GetBoneMatrixSig, sizeof(GetBoneMatrixSig)));
		if (!GetBoneMatrixF) return false;
	}

	UObjects::Init();
	return true;
}
