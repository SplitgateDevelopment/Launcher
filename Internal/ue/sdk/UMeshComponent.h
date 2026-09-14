#pragma once

#include "Fwd.h"
#include "Values.h"
#include "UPrimitiveComponent.h"

// Class Engine.MeshComponent
// Size: 0x480 (Inherited: 0x450)
struct UMeshComponent : UPrimitiveComponent
{
	struct TArray<struct UMaterialInterface*> OverrideMaterials; // 0x450(0x10)
	char pad_460[0x10];											 // 0x460(0x10)
	char bEnableMaterialParameterCaching : 1;					 // 0x470(0x01)
	char pad_470_1 : 7;											 // 0x470(0x01)
	char pad_471[0xf];											 // 0x471(0x0f)
};
