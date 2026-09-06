/// @file
/// @brief Out-of-line UFunction wrappers for AController.

#include "../Engine.h"

using namespace Engine;

void AController::SetControlRotation(FRotator& NewRotation)
{
	static auto Function = ObjObjects->FindObject("Function Engine.Controller.SetControlRotation");
	if (!Function) return;

	struct
	{
		FRotator NewRotation;
	} Parameters;
	Parameters.NewRotation = NewRotation;

	ProcessEvent(Function, &Parameters);
}
