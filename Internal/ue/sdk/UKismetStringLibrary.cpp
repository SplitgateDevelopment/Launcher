/// @file
/// @brief Out-of-line UFunction wrappers for UKismetStringLibrary.

#include "../Engine.h"

using namespace Engine;

struct FName UKismetStringLibrary::Conv_StringToName(struct FString InString)
{
	auto Func = ObjObjects->FindObject("Function Engine.KismetStringLibrary.Conv_StringToName");

	struct
	{
		struct FString InString;
		struct FName ReturnValue;
	} Parms;

	Parms.InString = InString;

	ProcessEvent(Func, &Parms);

	return Parms.ReturnValue;
};
