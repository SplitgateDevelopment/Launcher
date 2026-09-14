/// @file
/// @brief Out-of-line UFunction wrappers for ABaseGun.

#include "../Engine.h"

using namespace Engine;

void ABaseGun::UpdateSkins()
{
	static auto Function = GObjects->FindObject("Function PortalWars.BaseGun.UpdateSkins");
	if (!Function) return;

	ProcessEvent(Function, nullptr);
}
