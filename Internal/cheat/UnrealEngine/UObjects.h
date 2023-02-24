#pragma once
#include "Engine.h"

namespace UObjects {
	UObject* SwitchLevelUFunc = 0;
	UObject* FOVUFunc = 0;
	UObject* SetNameUFunc = 0;
	UObject* K2_DrawLineUFunc = 0;
	UObject* K2_DrawTextUFunc = 0;
	UObject* Font = 0;
	UObject* GetSaveGameUFUnc = 0;
	UObject* K2_GetPawnUFunc = 0;

	void Init() {
		SwitchLevelUFunc = ObjObjects->FindObject("Function Engine.PlayerController.SwitchLevel");
		FOVUFunc = ObjObjects->FindObject("Function Engine.PlayerController.FOV");
		SetNameUFunc = ObjObjects->FindObject("Function Engine.PlayerController.SetName");
		K2_DrawLineUFunc = ObjObjects->FindObject("Function Engine.Canvas.K2_DrawLine");
		K2_DrawTextUFunc = ObjObjects->FindObject("Function Engine.Canvas.K2_DrawText");
		Font = ObjObjects->FindObject("Font Roboto.Roboto");
		GetSaveGameUFUnc = ObjObjects->FindObject("Function PortalWars.PortalWarsLocalPlayer.GetUserSaveGame");
		K2_GetPawnUFunc = ObjObjects->FindObject("Function Engine.Controller.K2_GetPawn");
	}
}