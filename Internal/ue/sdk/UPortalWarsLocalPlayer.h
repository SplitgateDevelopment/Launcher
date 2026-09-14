#pragma once

#include "Fwd.h"
#include "Values.h"
#include "ULocalPlayer.h"

// Class PortalWars.PortalWarsLocalPlayer
// Size: 0x5e8 (Inherited: 0x258)
struct UPortalWarsLocalPlayer : ULocalPlayer
{
	char pad_258[0x30];												 // 0x258(0x30)
	struct UTexture* PlayerAvatar;									 // 0x288(0x08)
	char pad_290[0x228];											 // 0x290(0x228)
	struct TArray<struct UObject*> CachedCustomizationObjects;		 // 0x4b8(0x10)
	char pad_4C8[0x30];												 // 0x4c8(0x30)
	struct UPortalWarsNotificationManager* NotificationManager;		 // 0x4f8(0x08)
	struct UPortalWarsNotificationManager* NotificationManagerClass; // 0x500(0x08)
	struct UPortalWarsInviteManager* InviteManager;					 // 0x508(0x08)
	struct UPortalWarsInviteManager* InviteManagerClass;			 // 0x510(0x08)
	char ControllerDisconnectedScene[0x08];							 // 0x518(0x08)
	char ControllerDisconnectData[0x58];							 // 0x520(0x58)
	char pad_578[0x38];												 // 0x578(0x38)
	struct UPortalWarsSaveGame* UserSaveGameData;					 // 0x5b0(0x08)
	char pad_5B8[0x30];												 // 0x5b8(0x30)

	void SetPlayerAvatar(struct UTexture* avatar); // Function PortalWars.PortalWarsLocalPlayer.SetPlayerAvatar // (Final|Native|Protected) // @ game+0x16a7ef0
	void OnUserCustomizationsUpdate();			   // Function PortalWars.PortalWarsLocalPlayer.OnUserCustomizationsUpdate // (Final|Native|Protected) // @ game+0x16a7eb0
	void OnUserChosenCustomizationsUpdate();	   // Function PortalWars.PortalWarsLocalPlayer.OnUserChosenCustomizationsUpdate // (Final|Native|Protected) // @ game+0x16a7e90
	void OnCustomizationsLoaded();				   // Function PortalWars.PortalWarsLocalPlayer.OnCustomizationsLoaded // (Final|Native|Protected) // @ game+0x16a7b50
	void LoadUserSaveGame();					   // Function PortalWars.PortalWarsLocalPlayer.LoadUserSaveGame // (Final|Native|Public) // @ game+0x16a7af0
	struct UPortalWarsSaveGame* GetUserSaveGame(); // Function PortalWars.PortalWarsLocalPlayer.GetUserSaveGame // (Final|Native|Public|Const) // @ game+0x16a7910
};
