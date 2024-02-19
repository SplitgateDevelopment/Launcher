#pragma once

#include "../../ue/Engine.h"
#include "../../menu/Menu.h"
#include "../../settings/Settings.h"

void PostRender(UGameViewportClient* UGameViewportClient, Canvas* canvas)
{
	do {
		if (Settings.MISC.Unload) {
			Settings.MISC.Unload = false;
			return Hook::UnHook();
		}

		ZeroGUI::SetupCanvas(canvas);
		Menu::Tick();

		UWorld* World = *(UWorld**)(WRLD);
		if (!World) break;

		UGameInstance* OwningGameInstance = World->OwningGameInstance;
		if (!OwningGameInstance) break;

		TArray<UPlayer*> LocalPlayers = OwningGameInstance->LocalPlayers;

		UPlayer* LocalPlayer = LocalPlayers[0];
		if (!LocalPlayer) break;

		APlayerController* PlayerController = LocalPlayer->PlayerController;
		if (!PlayerController) break;

		//hook->features->handle(PlayerController);
	} while (false);

	return Hook::OriginalPostRender(UGameViewportClient, canvas);
}