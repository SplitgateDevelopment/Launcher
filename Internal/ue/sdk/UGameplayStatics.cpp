/// @file
/// @brief Out-of-line UFunction wrappers for UGameplayStatics.

#include "../Engine.h"

using namespace Engine;

struct UObject* UGameplayStatics::SpawnObject(struct UObject* ObjectClass, struct UObject* Outer)
{
	UObject* SpawnObject = ObjObjects->FindObject("Function Engine.GameplayStatics.SpawnObject");

	struct
	{
		UObject* ObjectClass;
		UObject* Outer;
		UObject* ReturnValue;
	} Parameters;

	Parameters.ObjectClass = ObjectClass;
	Parameters.Outer = Outer;

	ProcessEvent(SpawnObject, &Parameters);

	return Parameters.ReturnValue;
}
