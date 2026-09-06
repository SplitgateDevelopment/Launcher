/// @file
/// @brief Definitions of the cached UFunction/UObject handles and their one-time resolve.

#include "UObjects.h"

namespace Engine
{
	namespace UObjects
	{
		UObject* SwitchLevelUFunc = nullptr;
		UObject* FOVUFunc = nullptr;
		UObject* SetNameUFunc = nullptr;
		UObject* K2_DrawLineUFunc = nullptr;
		UObject* K2_DrawTextUFunc = nullptr;
		UObject* Font = nullptr;
		UObject* GetSaveGameUFUnc = nullptr;
		UObject* K2_GetPawnUFunc = nullptr;
		UObject* K2_SetRelativeRotationUFunc = nullptr;
		UObject* K2_TeleportTo = nullptr;
		UObject* K2_GetActorLocation = nullptr;
		UObject* K2_GetActorRotation = nullptr;
		UObject* SetActorEnableCollision = nullptr;
		UObject* GetActorEnableCollision = nullptr;
		UObject* K2_SetActorLocation = nullptr;

		void Init()
		{
			SwitchLevelUFunc = ObjObjects->FindObject("Function Engine.PlayerController.SwitchLevel");
			FOVUFunc = ObjObjects->FindObject("Function Engine.PlayerController.FOV");
			SetNameUFunc = ObjObjects->FindObject("Function Engine.PlayerController.SetName");
			K2_DrawLineUFunc = ObjObjects->FindObject("Function Engine.Canvas.K2_DrawLine");
			K2_DrawTextUFunc = ObjObjects->FindObject("Function Engine.Canvas.K2_DrawText");
			Font = ObjObjects->FindObject("Font Roboto.Roboto");
			GetSaveGameUFUnc = ObjObjects->FindObject("Function PortalWars.PortalWarsLocalPlayer.GetUserSaveGame");
			K2_GetPawnUFunc = ObjObjects->FindObject("Function Engine.Controller.K2_GetPawn");
			K2_SetRelativeRotationUFunc = ObjObjects->FindObject("Function Engine.SceneComponent.K2_SetRelativeRotation");
			K2_TeleportTo = ObjObjects->FindObject("Function Engine.Actor.K2_TeleportTo");
			K2_GetActorLocation = ObjObjects->FindObject("Function Engine.Actor.K2_GetActorLocation");
			K2_GetActorRotation = ObjObjects->FindObject("Function Engine.Actor.K2_GetActorRotation");
			SetActorEnableCollision = ObjObjects->FindObject("Function Engine.Actor.SetActorEnableCollision");
			GetActorEnableCollision = ObjObjects->FindObject("Function Engine.Actor.GetActorEnableCollision");
			K2_SetActorLocation = ObjObjects->FindObject("Function Engine.Actor.K2_SetActorLocation");
		}
	} // namespace UObjects
} // namespace Engine
