/// @file
/// @brief Out-of-line UFunction wrappers for USkinnedMeshComponent.

#include "../Engine.h"
#include "../UObjects.h"

using namespace Engine;

struct FName USkinnedMeshComponent::GetBoneName(int32_t BoneIndex)
{
	static auto Function = ObjObjects->FindObject("Function Engine.SkinnedMeshComponent.GetBoneName");
	struct
	{
		int32_t BoneIndex;
		FName ReturnValue;
	} Parameters;

	Parameters.BoneIndex = BoneIndex;

	ProcessEvent(Function, &Parameters);

	return Parameters.ReturnValue;
}
