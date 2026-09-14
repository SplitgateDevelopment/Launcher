/// @file
/// @brief Out-of-line UFunction wrappers for USceneComponent.

#include "../Engine.h"

using namespace Engine;

void USceneComponent::K2_SetRelativeRotation(struct FRotator NewRotation, bool bSweep, struct FHitResult& SweepHitResult, bool bTeleport)
{
	struct
	{
		FRotator NewRotation;
		bool bSweep;
		FHitResult SweepHitResult;
		bool bTeleport;
	} Parameters;
	Parameters.NewRotation = NewRotation;
	Parameters.bSweep = bSweep;
	Parameters.SweepHitResult = SweepHitResult;
	Parameters.bTeleport = bTeleport;

	static auto Function = GObjects->FindObject("Function Engine.SceneComponent.K2_SetRelativeRotation");
	ProcessEvent(Function, &Parameters);
}
