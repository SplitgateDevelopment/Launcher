/// @file
/// @brief Out-of-line UFunction wrappers for UPrimitiveComponent.

#include "../Engine.h"

using namespace Engine;

void UPrimitiveComponent::SetRenderCustomDepth(bool bValue)
{
	static auto Function = ObjObjects->FindObject("Function Engine.PrimitiveComponent.SetRenderCustomDepth");
	if (!Function) return;

	struct
	{
		bool bValue;
	} Parameters;
	Parameters.bValue = bValue;

	ProcessEvent(Function, &Parameters);
}
