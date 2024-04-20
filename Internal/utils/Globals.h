#pragma once

#include "../ue/Engine.h"

namespace Globals {
	UEngine* Engine = 0;
	UWorld* World = 0;
	APortalWarsPlayerController* PlayerController = 0;
	UGameplayStatics* GameplayStatics = 0;
	UKismetStringLibrary* KismetStringLibrary = 0;
	UKismetTextLibrary* KismetTextLibrary = 0;

	void Init() {
		Engine = UEngine::GetEngine();
		World = UWorld::GetWorld();
		GameplayStatics = reinterpret_cast<UGameplayStatics*>(UGameplayStatics::StaticClass());
		KismetStringLibrary = reinterpret_cast<UKismetStringLibrary*>(UKismetStringLibrary::StaticClass());
		KismetTextLibrary = reinterpret_cast<UKismetTextLibrary*>(UKismetTextLibrary::StaticClass());
	}
}