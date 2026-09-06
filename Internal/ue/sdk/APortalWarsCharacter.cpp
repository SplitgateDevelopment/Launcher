/// @file
/// @brief Out-of-line UFunction wrappers for APortalWarsCharacter.

#include "../Engine.h"
#include "../UObjects.h"

using namespace Engine;

void APortalWarsCharacter::UpdateSkins()
{
	static auto Function = ObjObjects->FindObject("Function PortalWars.PortalWarsCharacter.UpdateSkins");
	if (!Function) return;

	ProcessEvent(Function, nullptr);
}
void APortalWarsCharacter::RequestSuicide()
{
	static auto Function = ObjObjects->FindObject("Function PortalWars.PortalWarsCharacter.RequestSuicide");
	if (!Function) return;

	ProcessEvent(Function, nullptr);
}
char APortalWarsCharacter::GetTeamNum()
{
	static auto Function = ObjObjects->FindObject("Function PortalWars.PortalWarsCharacter.GetTeamNum");

	struct
	{
		char ReturnValue;
	} Parameters;

	ProcessEvent(Function, &Parameters);

	return Parameters.ReturnValue;
}
