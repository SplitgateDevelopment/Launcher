#include "Engine.h"
#include "Util.h"
#include <Psapi.h>

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

FNamePool* NamePoolData = nullptr;
TUObjectArray* ObjObjects = nullptr;
UWorld* WRLD = nullptr;

UObject* SwitchLevelUFunc;
UObject* EnableCheatsUFunc;
UObject* FOVUFunc;
UObject* SetNameUFunc;

uintptr_t GetBoneMatrixF;

void(*OPostRender)(UGameViewportClient* UGameViewportClient, Canvas* Canvas) = nullptr;

void APlayerController::SwitchLevel(FString URL)
{
	struct {
		FString URL;
	} Parameters;
	Parameters.URL = URL;

	ProcessEvent(SwitchLevelUFunc, &Parameters);
}

void APlayerController::EnableCheats()
{
	struct {} Parameters;
	ProcessEvent(EnableCheatsUFunc, &Parameters);
}

void APlayerController::FOV(float NewFOV)
{
	struct {
		float NewFOV;
	} Parameters;
	Parameters.NewFOV = NewFOV;

	ProcessEvent(FOVUFunc, &Parameters);
}

void APlayerController::SetName(FString S)
{
	struct {
		FString S;
	} Parameters;
	Parameters.S = S;

	ProcessEvent(SetNameUFunc, &Parameters);
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

	SwitchLevelUFunc = ObjObjects->FindObject("Function Engine.PlayerController.SwitchLevel");
	EnableCheatsUFunc = ObjObjects->FindObject("Function Engine.PlayerController.EnableCheats");
	FOVUFunc = ObjObjects->FindObject("Function Engine.PlayerController.FOV");
	SetNameUFunc = ObjObjects->FindObject("Function Engine.PlayerController.SetName");

	return true;
}