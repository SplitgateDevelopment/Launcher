/// @file
/// @brief Out-of-line UFunction wrappers for ACharacter.

#include "../Engine.h"

using namespace Engine;

void ACharacter::LaunchCharacter(FVector LaunchVelocity, bool bXYOverride, bool bZOverride)
{
	static auto Function = GObjects->FindObject("Function Engine.Character.LaunchCharacter");
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
