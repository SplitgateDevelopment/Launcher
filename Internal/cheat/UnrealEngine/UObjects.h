#pragma once
#include "Engine.h"

namespace UObjects {
	UObject* SwitchLevelUFunc;
	UObject* FOVUFunc;
	UObject* SetNameUFunc;
	UObject* K2_DrawLineUFunc;
	UObject* K2_DrawTextUFunc;
	UObject* Font;
	UObject* GetSaveGameUFUnc;
	UObject* K2_GetPawnUFunc;

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