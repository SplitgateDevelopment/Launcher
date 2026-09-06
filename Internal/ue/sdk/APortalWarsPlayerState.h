#pragma once

#include "Fwd.h"
#include "Values.h"
#include "APlayerState.h"

// Class PortalWars.PortalWarsPlayerState
// Size: 0xb50 (Inherited: 0x320)
struct APortalWarsPlayerState : APlayerState
{
	char pad_320[0x18];										   // 0x320(0x18)
	char TeamNum;											   // 0x338(0x01)
	char pad_339[0x1];										   // 0x339(0x01)
	uint16_t ProgressTowardsScoring;						   // 0x33a(0x02)
	char pad_33C[0x1c];										   // 0x33c(0x1c)
	char KillStreak;										   // 0x358(0x01)
	char MultiKillCounter;									   // 0x359(0x01)
	char pad_35A[0x5e];										   // 0x35a(0x5e)
	char DamagePlayers[0x50];								   // 0x3b8(0x50)
	char pad_408[0xf0];										   // 0x408(0xf0)
	struct FPlayerStatsInfo_InDepth PlayerStats;			   // 0x4f8(0xc8)
	char PostGameStats[0x268];								   // 0x5c0(0x268)
	char pad_828[0x18];										   // 0x828(0x18)
	bool bIsAdmin;											   // 0x840(0x01)
	char pad_841[0x7];										   // 0x841(0x07)
	struct FEquippedCustomizations PlayerCustomizations;	   // 0x848(0x10)
	char pad_858[0x30];										   // 0x858(0x30)
	struct TArray<struct UObject*> CachedCustomizationObjects; // 0x888(0x10)
	char pad_898[0x38];										   // 0x898(0x38)
	struct UTexture* PlayerAvatar;							   // 0x8d0(0x08)
	struct UTexture* SavedPlayerAvatar;						   // 0x8d8(0x08)
	struct UTexture* SavedBotAvatar;						   // 0x8e0(0x08)
	struct TArray<struct FUserRankInfo> PlayerRanks;		   // 0x8e8(0x10)
	char pad_8F8[0x18];										   // 0x8f8(0x18)
	char Progression[0x10];									   // 0x910(0x10)
	char pad_920[0x18];										   // 0x920(0x18)
	char DailyStreak[0x18];									   // 0x938(0x18)
	char pad_950[0x18];										   // 0x950(0x18)
	char BadgeProgress[0x10];								   // 0x968(0x10)
	char pad_978[0x50];										   // 0x978(0x50)
	bool bHideNames;										   // 0x9c8(0x01)
	char bAnonymousMode : 1;								   // 0x9c9(0x01)
	char bPartyChatOnly : 1;								   // 0x9c9(0x01)
	char pad_9C9_2 : 6;										   // 0x9c9(0x01)
	char pad_9CA[0x26];										   // 0x9ca(0x26)
	struct APortalWarsCharacter* CachedCharacter;			   // 0x9f0(0x08)
	char bQuitter : 1;										   // 0x9f8(0x01)
	char pad_9F8_1 : 7;										   // 0x9f8(0x01)
	char pad_9F9[0x1];										   // 0x9f9(0x01)
	uint16_t InactiveTime;									   // 0x9fa(0x02)
	bool bJoinedInProgress;									   // 0x9fc(0x01)
	char pad_9FD[0x28];										   // 0x9fd(0x28)
	bool bIsFlagged;										   // 0xa25(0x01)
	char pad_A26[0x1a];										   // 0xa26(0x1a)
	bool bIsReadyForNextMatch;								   // 0xa40(0x01)
	char pad_A41[0x37];										   // 0xa41(0x37)
	char DefaultProfile[0xc0];								   // 0xa78(0xc0)
	float SimulatedFOV;										   // 0xb38(0x04)
	char pad_B3C[0x14];										   // 0xb3c(0x14)

	void SetTeamNum(char NewTeamNumber);																																																																												   // Function PortalWars.PortalWarsPlayerState.SetTeamNum // (Final|Native|Public|BlueprintCallable) // @ game+0x16be440
	void SetPlayerAvatar(struct UTexture* avatar);																																																																										   // Function PortalWars.PortalWarsPlayerState.SetPlayerAvatar // (Final|Native|Public) // @ game+0x16be230
	void ServerUpdateLobbySessionId(struct FString NewLobbySessionId);																																																																					   // Function PortalWars.PortalWarsPlayerState.ServerUpdateLobbySessionId // (Net|NetReliableNative|Event|Protected|NetServer) // @ game+0x16be170
	void ServerChangeTeam();																																																																															   // Function PortalWars.PortalWarsPlayerState.ServerChangeTeam // (Net|NetReliableNative|Event|Protected|NetServer|NetValidate) // @ game+0x16bd730
	void OnRep_Team();																																																																																	   // Function PortalWars.PortalWarsPlayerState.OnRep_Team // (Native|Protected) // @ game+0x169a850
	void OnRep_ReadyForNextMatch();																																																																														   // Function PortalWars.PortalWarsPlayerState.OnRep_ReadyForNextMatch // (Final|Native|Protected) // @ game+0x16bd1a0
	void OnRep_Ranks();																																																																																	   // Function PortalWars.PortalWarsPlayerState.OnRep_Ranks // (Final|Native|Protected) // @ game+0x16bd180
	void OnRep_Progression();																																																																															   // Function PortalWars.PortalWarsPlayerState.OnRep_Progression // (Final|Native|Protected) // @ game+0x16bd160
	void OnRep_PlayerStats();																																																																															   // Function PortalWars.PortalWarsPlayerState.OnRep_PlayerStats // (Final|Native|Protected) // @ game+0x16bd140
	void OnRep_PlayerCustomizations();																																																																													   // Function PortalWars.PortalWarsPlayerState.OnRep_PlayerCustomizations // (Final|Native|Protected) // @ game+0x16bd120
	void OnRep_PartyChatOnly();																																																																															   // Function PortalWars.PortalWarsPlayerState.OnRep_PartyChatOnly // (Final|Native|Protected) // @ game+0x16bd100
	void OnRep_MultiKillCounter(char OldMultiKillCounter);																																																																								   // Function PortalWars.PortalWarsPlayerState.OnRep_MultiKillCounter // (Final|Native|Protected) // @ game+0x16bd080
	void OnRep_KillStreak(char OldKillStreak);																																																																											   // Function PortalWars.PortalWarsPlayerState.OnRep_KillStreak // (Final|Native|Protected) // @ game+0x16bd000
	void OnRep_JoinedInProgress();																																																																														   // Function PortalWars.PortalWarsPlayerState.OnRep_JoinedInProgress // (Final|Native|Protected) // @ game+0x16bcfe0
	void OnRep_IsFlagged();																																																																																   // Function PortalWars.PortalWarsPlayerState.OnRep_IsFlagged // (Final|Native|Protected) // @ game+0x16bcfc0
	void OnRep_IsAdmin();																																																																																   // Function PortalWars.PortalWarsPlayerState.OnRep_IsAdmin // (Final|Native|Protected) // @ game+0x16bcfa0
	void OnRep_DefaultProfile();																																																																														   // Function PortalWars.PortalWarsPlayerState.OnRep_DefaultProfile // (Final|Native|Protected) // @ game+0x16bcf80
	void OnRep_DailyStreak();																																																																															   // Function PortalWars.PortalWarsPlayerState.OnRep_DailyStreak // (Final|Native|Protected) // @ game+0x16bcf60
	void OnRep_BadgeProgress();																																																																															   // Function PortalWars.PortalWarsPlayerState.OnRep_BadgeProgress // (Final|Native|Protected) // @ game+0x16bcf40
	void OnRep_AnonymousMode();																																																																															   // Function PortalWars.PortalWarsPlayerState.OnRep_AnonymousMode // (Final|Native|Protected) // @ game+0x16bcf20
	void OnPlayerAvatarLoaded(struct UTexture* avatar);																																																																									   // Function PortalWars.PortalWarsPlayerState.OnPlayerAvatarLoaded // (Final|Native|Public) // @ game+0x16bcda0
	void OnDefaultAvatarLoaded();																																																																														   // Function PortalWars.PortalWarsPlayerState.OnDefaultAvatarLoaded // (Final|Native|Protected) // @ game+0x16bcd20
	void OnCustomizationsLoaded();																																																																														   // Function PortalWars.PortalWarsPlayerState.OnCustomizationsLoaded // (Final|Native|Protected) // @ game+0x16bcd00
	void LoadPlayerAvatar();																																																																															   // Function PortalWars.PortalWarsPlayerState.LoadPlayerAvatar // (Final|Native|Protected) // @ game+0x16bcc80
	char GetTeamNum(bool bUseKillerDuringKillcam);																																																																										   // Function PortalWars.PortalWarsPlayerState.GetTeamNum // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x16bcb80
	struct FString GetShortPlayerName();																																																																												   // Function PortalWars.PortalWarsPlayerState.GetShortPlayerName // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x16bca30
	void ClientTriggerEvents(struct TArray<struct FName> EventNames);																																																																					   // Function PortalWars.PortalWarsPlayerState.ClientTriggerEvents // (Net|Native|Event|Protected|NetClient) // @ game+0x16bc510
	void ClientTriggerEvent(struct FName EventName);																																																																									   // Function PortalWars.PortalWarsPlayerState.ClientTriggerEvent // (Net|Native|Event|Protected|NetClient) // @ game+0x16bc480
	void ClientReconnected();																																																																															   // Function PortalWars.PortalWarsPlayerState.ClientReconnected // (Net|NetReliableNative|Event|Protected|NetClient) // @ game+0x1695cd0
	void ClientReceivedMedals(struct TArray<struct FName> MedalNames);																																																																					   // Function PortalWars.PortalWarsPlayerState.ClientReceivedMedals // (Net|Native|Event|Protected|NetClient) // @ game+0x16bc080
	void ClientReceivedMedal(struct FName MedalName);																																																																									   // Function PortalWars.PortalWarsPlayerState.ClientReceivedMedal // (Net|Native|Event|Protected|NetClient) // @ game+0x16bbff0
	void BroadcastDeath_Multicast(struct APortalWarsPlayerState* KillerPlayerState, struct UDamageType* KillerDamageType, struct APortalWarsPlayerState* KilledPlayerState, bool bIsHeadshot, bool bThruPortal, float KillerLastTeleportTime, struct TArray<struct APortalWarsPlayerState*> DamagePlayerStates, struct TArray<float> DamagePlayerDamages); // Function PortalWars.PortalWarsPlayerState.BroadcastDeath_Multicast // (Net|NetReliableNative|Event|NetMulticast|Public) // @ game+0x16bb600
};
