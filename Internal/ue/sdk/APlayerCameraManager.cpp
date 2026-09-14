/// @file
/// @brief Out-of-line UFunction wrappers for APlayerCameraManager.

#include "../Engine.h"

using namespace Engine;

FVector APlayerCameraManager::GetCameraLocation()
{
	static auto Function = GObjects->FindObject("Function Engine.PlayerCameraManager.GetCameraLocation");

	struct
	{
		FVector ReturnValue;
	} Parameters;

	ProcessEvent(Function, &Parameters);

	return Parameters.ReturnValue;
};
FRotator APlayerCameraManager::GetCameraRotation()
{
	static auto Function = GObjects->FindObject("Function Engine.PlayerCameraManager.GetCameraRotation");

	struct
	{
		FRotator ReturnValue;
	} Parameters;

	ProcessEvent(Function, &Parameters);

	return Parameters.ReturnValue;
};
float APlayerCameraManager::GetFOVAngle()
{
	static auto Function = GObjects->FindObject("Function Engine.PlayerCameraManager.GetFOVAngle");

	struct
	{
		float ReturnValue;
	} Parameters;

	ProcessEvent(Function, &Parameters);

	return Parameters.ReturnValue;
};
