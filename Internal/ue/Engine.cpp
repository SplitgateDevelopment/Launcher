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

void Canvas::K2_DrawLine(FVector2D ScreenPositionA, FVector2D ScreenPositionB, FLOAT Thickness, FLinearColor Color)
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

void Canvas::K2_DrawText(FString RenderText, FVector2D ScreenPosition, FVector2D Scale, FLinearColor RenderColor, float Kerning, FLinearColor ShadowColor, FVector2D ShadowOffset, bool bCentreX, bool bCentreY, bool bOutlined, FLinearColor OutlineColor)
{
	struct {
		UObject* RenderFont; //UFont* 
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

	Parameters.RenderFont = UObjects::Font;
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

void APortalWarsPlayerController::ClientUpdateChat(struct FTextChatData InData)
{
	auto Function = ObjObjects->FindObject("Function PortalWars.PortalWarsPlayerController.ClientUpdateChat");

	struct {
		struct FTextChatData InData;
	} Params;
	Params.InData = InData;

	UObject::ProcessEvent(Function, &Params);
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

	UObjects::Init();
	return true;
}
