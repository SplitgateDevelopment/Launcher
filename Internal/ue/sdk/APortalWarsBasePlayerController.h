#pragma once

#include "Fwd.h"
#include "Values.h"
#include "APlayerController.h"

// Class PortalWars.PortalWarsBasePlayerController
// Size: 0x588 (Inherited: 0x570)
struct APortalWarsBasePlayerController : APlayerController
{
	char pad_570[0x18]; // 0x570(0x18)

	void ServerKickForInactivity(); // Function PortalWars.PortalWarsBasePlayerController.ServerKickForInactivity // (Net|NetReliableNative|Event|Protected|NetServer|NetValidate) // @ game+0x16616b0
	void KickForInactivity();		// Function PortalWars.PortalWarsBasePlayerController.KickForInactivity // (Final|Native|Protected) // @ game+0x1661560
};
