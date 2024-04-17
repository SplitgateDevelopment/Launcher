#pragma once

#include "../../ue/Engine.h"
#include "../../menu/Menu.h"
#include "../../settings/Settings.h"

void PostRender(UGameViewportClient* UGameViewportClient, Canvas* canvas)
{
	do {
		UWorld* World = *(UWorld**)(WRLD);
		if (!World) break;

		UGameInstance* OwningGameInstance = World->OwningGameInstance;
		if (!OwningGameInstance) break;

		TArray<ULocalPlayer*> LocalPlayers = OwningGameInstance->LocalPlayers;

		UPortalWarsLocalPlayer* LocalPlayer = (UPortalWarsLocalPlayer*)LocalPlayers[0];
		if (!LocalPlayer) break;

		APlayerController* PlayerController = LocalPlayer->PlayerController;
		if (!PlayerController) break;

		Hook::features->handle(PlayerController);
	} while (false);

	return Hook::PostRender::Original(UGameViewportClient, canvas);
}