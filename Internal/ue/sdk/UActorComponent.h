#pragma once

#include "Fwd.h"
#include "Values.h"
#include "UObject.h"

// Class Engine.ActorComponent
// Size: 0xb0 (Inherited: 0x28)
struct UActorComponent : UObject
{
	char pad_28[0x8];													// 0x28(0x08)
	char PrimaryComponentTick[0x30];									// 0x30(0x30)
	struct TArray<struct FName> ComponentTags;							// 0x60(0x10)
	struct TArray<struct UAssetUserData*> AssetUserData;				// 0x70(0x10)
	char pad_80[0x4];													// 0x80(0x04)
	int32_t UCSSerializationIndex;										// 0x84(0x04)
	char pad_88_0 : 3;													// 0x88(0x01)
	char bNetAddressable : 1;											// 0x88(0x01)
	char bReplicates : 1;												// 0x88(0x01)
	char pad_88_5 : 3;													// 0x88(0x01)
	char pad_89_0 : 7;													// 0x89(0x01)
	char bAutoActivate : 1;												// 0x89(0x01)
	char bIsActive : 1;													// 0x8a(0x01)
	char bEditableWhenInherited : 1;									// 0x8a(0x01)
	char pad_8A_2 : 1;													// 0x8a(0x01)
	char bCanEverAffectNavigation : 1;									// 0x8a(0x01)
	char pad_8A_4 : 1;													// 0x8a(0x01)
	char bIsEditorOnly : 1;												// 0x8a(0x01)
	char pad_8A_6 : 2;													// 0x8a(0x01)
	char pad_8B[0x1];													// 0x8b(0x01)
	char CreationMethod[0x01];											// 0x8c(0x01)
	char OnComponentActivated[0x01];									// 0x8d(0x01)
	char OnComponentDeactivated[0x01];									// 0x8e(0x01)
	char pad_8F[0x1];													// 0x8f(0x01)
	struct TArray<struct FSimpleMemberReference> UCSModifiedProperties; // 0x90(0x10)
	char pad_A0[0x10];													// 0xa0(0x10)
};
