/// @file
/// @brief Definitions of the cached engine-object pointers and their one-time resolve.

#include "Globals.h"

namespace Globals
{
	UEngine* Engine = nullptr;
	UWorld* World = nullptr;
	APortalWarsPlayerController* PlayerController = nullptr;
	UGameplayStatics* GameplayStatics = nullptr;
	UCanvas* Canvas = nullptr;

	void Init()
	{
		Engine = UEngine::GetEngine();
		World = UWorld::GetWorld();
		GameplayStatics = reinterpret_cast<UGameplayStatics*>(UGameplayStatics::StaticClass());
	}
} // namespace Globals
