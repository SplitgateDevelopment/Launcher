#pragma once

#include "Fwd.h"

// Hand-added (not in the Dumpspace dump) for UGameplayStatics::SpawnActor. FTransform is laid out to
// match UE4's 0x30-byte struct (Rotation @0x00, Translation @0x10, Scale3D @0x20).
struct FQuat
{
	float X, Y, Z, W;
};
