/// @file
/// @brief Out-of-line UFunction wrappers for APortalWarsPlayerController.

#include "../Engine.h"

using namespace Engine;

void APortalWarsPlayerController::ClientUpdateChat(struct FTextChatData InData)
{
	auto Function = GObjects->FindObject("Function PortalWars.PortalWarsPlayerController.ClientUpdateChat");

	struct
	{
		struct FTextChatData InData;
	} Params;
	Params.InData = InData;

	UObject::ProcessEvent(Function, &Params);
}
void APortalWarsPlayerController::SendChatMessage(FString Message, enum class EChatType ChatType)
{
	if (!this->IsInGame()) return;

	FString sender = FString("[Splitgate Internal]");
	FTextChatData ChatData{};

	ChatData.SenderName = sender;
	ChatData.SenderText = Message;
	ChatData.NiceText = Message;
	ChatData.ChatType = ChatType;
	ChatData.SenderID = {};

	this->ClientUpdateChat(ChatData);
}
bool APortalWarsPlayerController::ProjectWorldLocationToScreenCustom(FVector WorldLocation, FVector2D& ScreenLocation, bool bPlayerViewportRelative)
{
	static auto Function = GObjects->FindObject("Function PortalWars.PortalWarsPlayerController.ProjectWorldLocationToScreenCustom");
	if (!Function) return false;

	struct
	{
		FVector WorldLocation;			 // 0x00
		FVector2D ScreenLocation;		 // 0x0c (out)
		bool bPlayerViewportRelative;	 // 0x14
		bool ReturnValue;				 // 0x15
	} Parameters;
	Parameters.WorldLocation = WorldLocation;
	Parameters.ScreenLocation = {};
	Parameters.bPlayerViewportRelative = bPlayerViewportRelative;

	ProcessEvent(Function, &Parameters);

	ScreenLocation = Parameters.ScreenLocation;
	return Parameters.ReturnValue;
}
void APortalWarsPlayerController::ClientSetSpectatorCamera(FVector CameraLocation, FRotator CameraRotation)
{
	static auto Function = GObjects->FindObject("Function PortalWars.PortalWarsPlayerController.ClientSetSpectatorCamera");
	if (!Function) return;

	struct
	{
		FVector CameraLocation;
		FRotator CameraRotation;
	} Parameters;
	Parameters.CameraLocation = CameraLocation;
	Parameters.CameraRotation = CameraRotation;

	ProcessEvent(Function, &Parameters);
}
