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

// The engine globals/UFunctions now live in namespace Engine; pull them into scope
// so the out-of-line SDK bodies below keep referencing them unqualified.
using namespace Engine;

FNamePool* Engine::NamePoolData = nullptr;
TUObjectArray* Engine::ObjObjects = nullptr;
UWorld* Engine::WRLD = nullptr;
uintptr_t Engine::GetBoneMatrixF;
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

	return true;
}
