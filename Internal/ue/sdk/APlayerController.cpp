/// @file
/// @brief Out-of-line UFunction wrappers for APlayerController.

#include "../Engine.h"

using namespace Engine;

void APlayerController::SwitchLevel(FString URL)
{
	struct
	{
		FString URL;
	} Parameters;
	Parameters.URL = URL;

	static auto Function = GObjects->FindObject("Function Engine.PlayerController.SwitchLevel");
	ProcessEvent(Function, &Parameters);
}
void APlayerController::FOV(float NewFOV)
{
	struct
	{
		float NewFOV;
	} Parameters;
	Parameters.NewFOV = NewFOV;

	static auto Function = GObjects->FindObject("Function Engine.PlayerController.FOV");
	ProcessEvent(Function, &Parameters);
}
void APlayerController::SetName(FString S)
{
	struct
	{
		FString S;
	} Parameters;
	Parameters.S = S;

	static auto Function = GObjects->FindObject("Function Engine.PlayerController.SetName");
	ProcessEvent(Function, &Parameters);
}
void APlayerController::ConsoleKey(struct FKey Key)
{
	auto Func = GObjects->FindObject("Function Engine.PlayerController.ConsoleKey");

	struct
	{
		struct FKey Key;
	} Parms;

	Parms.Key = Key;

	ProcessEvent(Func, &Parms);
};
void APlayerController::ClientSetCameraMode(struct FName NewCameraMode)
{
	auto Function = GObjects->FindObject("Function Engine.PlayerController.ClientSetCameraMode");

	struct
	{
		struct FName NewCameraMode;
	} Params;
	Params.NewCameraMode = NewCameraMode;

	UObject::ProcessEvent(Function, &Params);
}
void APlayerController::SendToConsole(FString Command)
{
	auto Function = GObjects->FindObject("Function Engine.PlayerController.SendToConsole");

	struct
	{
		FString Command;
	} Params;
	Params.Command = Command;

	UObject::ProcessEvent(Function, &Params);
}
bool APlayerController::IsInGame()
{
	// A pawn is acknowledged AND we're not in the post-match lobby controller. Excluding the
	// post-game controller here makes every feature + the actor cache stand down after a match
	// (they all gate on IsInGame), which is where the post-game FPS drop came from.
	return this->AcknowledgedPawn && !IsPostGameController(reinterpret_cast<UObject*>(this));
}
bool APlayerController::ProjectWorldLocationToScreen(FVector WorldLocation, FVector2D& ScreenLocation, bool bPlayerViewportRelative)
{
	// Resolve the UFunction once and reuse it. This is on the ESP hot path (one call per bone per
	// enemy per frame); doing the object-array lookup every call was the main ESP frame-rate cost.
	static auto Function = GObjects->FindObject("Function Engine.PlayerController.ProjectWorldLocationToScreen");

	struct
	{
		FVector WorldLocation;
		FVector2D ScreenLocation;
		bool bPlayerViewportRelative;
		bool ReturnValue;
	} Parameters;

	Parameters.WorldLocation = WorldLocation;
	Parameters.ScreenLocation = ScreenLocation;
	Parameters.bPlayerViewportRelative = bPlayerViewportRelative;

	ProcessEvent(Function, &Parameters);

	ScreenLocation = Parameters.ScreenLocation;

	return Parameters.ReturnValue;
};
void APlayerController::ClientMessage(FString S, FName Type, float MsgLifeTime)
{
	static auto Function = GObjects->FindObject("Function Engine.PlayerController.ClientMessage");
	if (!Function) return;

	struct
	{
		FString S;
		FName Type;
		float MsgLifeTime;
	} Parameters;
	Parameters.S = S;
	Parameters.Type = Type;
	Parameters.MsgLifeTime = MsgLifeTime;

	ProcessEvent(Function, &Parameters);
}
