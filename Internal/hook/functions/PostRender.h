#pragma once

#include "../../ue/Engine.h"
#include "../../settings/Settings.h"
#include "../../utils/Globals.h"

namespace PostRender {
	void** VTable;
	void(*Original)(UGameViewportClient* UGameViewportClient, UCanvas* Canvas) = nullptr;
	int Index = 100;

	void HookedPostRender(UGameViewportClient* UGameViewportClient, UCanvas* Canvas)
	{
		do {
			UWorld* World = UWorld::GetWorld();
			if (!World) break;

			UGameInstance* OwningGameInstance = World->OwningGameInstance;
			if (!OwningGameInstance) break;

			TArray<ULocalPlayer*> LocalPlayers = OwningGameInstance->LocalPlayers;

			UPortalWarsLocalPlayer* LocalPlayer = (UPortalWarsLocalPlayer*)LocalPlayers[0];
			if (!LocalPlayer) break;

			APlayerController* PlayerController = LocalPlayer->PlayerController;
			if (!PlayerController) break;


			Globals::World = World;
			Globals::Canvas = Canvas;
			Globals::PlayerController = (APortalWarsPlayerController*)PlayerController;

			try {
				for (const auto& Feature : Features::Features)
				{
					if (!Feature->Initialized)
					{
						Feature->Init();
					};

					if (!Feature->Check())
					{
						continue;
					};

					Feature->Enabled ? Feature->Run() : Feature->Destroy();
				}
			}
			catch (char* e) {
				Logger::Log("ERROR", "Failed to execute feature: " + std::string(e));
			}
		} while (false);

		return Original(UGameViewportClient, Canvas);
	}
}