#pragma once

#include "Fwd.h"
#include "Values.h"
#include "AActor.h"

// Class PortalWars.CharacterSkin (minimal — the skin owns the character's rendered 3P mesh, which is
// what the glow's custom-depth outline must be applied to).
struct ACharacterSkin : AActor
{
	struct USkeletalMeshComponent* GetMesh3P(); // Function PortalWars.(Base)CharacterSkin.GetMesh3P
};
