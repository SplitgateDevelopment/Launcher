/// @file
/// @brief Engine::Init — the one-time bootstrap that resolves the engine globals
/// (GObjects / GNames / GWorld / GetBoneMatrixFn) from byte signatures. The globals
/// themselves are inline variables in Engine.h; everything else in the SDK is header
/// -only or split across sdk/<Type>.cpp and custom.cpp.
#include "Engine.h"
#include "../memory/Memory.h"
#include <Psapi.h>
#include <cstddef>

using namespace Engine;

bool Engine::Init()
{
	auto main = GetModuleHandleA(nullptr);

	static byte gObjectsSig[] = {0x48, 0x8B, 0x05, 0x00, 0x00, 0x00, 0x00, 0x48, 0x8B, 0x0C, 0xC8, 0x48, 0x8D, 0x04, 0xD1, 0xEB};
	GObjects = reinterpret_cast<decltype(GObjects)>(Memory::FindPointer(main, gObjectsSig, sizeof(gObjectsSig), 0));
	if (!GObjects) return false;

	static byte gNamesSig[] = {0x48, 0x8D, 0x0D, 0x00, 0x00, 0x00, 0x00, 0xE8, 0x00, 0x00, 0x00, 0x00, 0xC6, 0x05, 0x00, 0x00, 0x00, 0x00, 0x01, 0x0F, 0x10, 0x03, 0x4C, 0x8D, 0x44, 0x24, 0x20, 0x48, 0x8B, 0xC8};
	GNames = reinterpret_cast<decltype(GNames)>(Memory::FindPointer(main, gNamesSig, sizeof(gNamesSig), 0));
	if (!GNames) return false;

	static byte gWorldSig[] = {0x48, 0x8B, 0x1D, 0x00, 0x00, 0x00, 0x00, 0x48, 0x85, 0xDB, 0x74, 0x3B, 0x41, 0xB0, 0x01, 0x33, 0xD2, 0x48, 0x8B, 0xCB, 0xE8};
	GWorld = reinterpret_cast<decltype(GWorld)>(Memory::FindPointer(main, gWorldSig, sizeof(gWorldSig), 0));
	if (!GWorld) return false;

	static byte getBoneMatrixSig[] = {0x48, 0x8B, 0xC4, 0x55, 0x53, 0x56, 0x57, 0x41, 0x54, 0x41, 0x56, 0x41, 0x57, 0x48, 0x8D, 0x68, 0xA1, 0x48, 0x81, 0xEC, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x29, 0x78, 0xB8, 0x33, 0xF6, 0x44, 0x0F, 0x29, 0x40};
	MODULEINFO info;
	if (K32GetModuleInformation(GetCurrentProcess(), main, &info, sizeof(MODULEINFO)))
	{
		auto base = static_cast<byte*>(info.lpBaseOfDll);
		GetBoneMatrixFn = reinterpret_cast<decltype(GetBoneMatrixFn)>(Memory::Find(base, base + info.SizeOfImage - 1, Memory::FromBytes(getBoneMatrixSig, sizeof(getBoneMatrixSig))));
		if (!GetBoneMatrixFn) return false;
	}

	return true;
}

void Engine::ResolveObjects()
{
	GEngine = UEngine::GetEngine();
	World = UWorld::GetWorld();
	GameplayStatics = reinterpret_cast<UGameplayStatics*>(Engine::StaticClass<UGameplayStatics>());
	KismetStringLibrary = reinterpret_cast<UKismetStringLibrary*>(Engine::StaticClass<UKismetStringLibrary>());
	KismetTextLibrary = reinterpret_cast<UKismetTextLibrary*>(Engine::StaticClass<UKismetTextLibrary>());
}
