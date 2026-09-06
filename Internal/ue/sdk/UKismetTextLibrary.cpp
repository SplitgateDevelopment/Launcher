/// @file
/// @brief Out-of-line UFunction wrappers for UKismetTextLibrary.

#include "../Engine.h"

using namespace Engine;

struct FText UKismetTextLibrary::Conv_StringToText(struct FString InString)
{
	auto Func = GObjects->FindObject("Function Engine.KismetStringLibrary.Conv_StringToText");

	struct
	{
		struct FString InString;
		struct FText ReturnValue;
	} Parms;

	Parms.InString = InString;

	ProcessEvent(Func, &Parms);

	return Parms.ReturnValue;
};
