#pragma once

#include "Fwd.h"
#include "Values.h"
#include "APortalWarsBasePlayerController.h"

// Class PortalWars.PortalWarsPlayerController
// Size: 0x740 (Inherited: 0x588)
struct APortalWarsPlayerController : APortalWarsBasePlayerController
{
	char pad_588[0x40];													  // 0x588(0x40)
	struct ALevelSequenceActor* LSActor;								  // 0x5c8(0x08)
	struct ULevelSequencePlayer* LSPlayer;								  // 0x5d0(0x08)
	char pad_5D8[0x50];													  // 0x5d8(0x50)
	struct APortal* leftPortal;											  // 0x628(0x08)
	struct APortal* rightPortal;										  // 0x630(0x08)
	struct APortalSceneCapture2D* leftPortalSceneCapture;				  // 0x638(0x08)
	struct APortalSceneCapture2D* rightPortalSceneCapture;				  // 0x640(0x08)
	struct APortalSceneCapture2D* portalSceneCaptureClass;				  // 0x648(0x08)
	struct UTextureRenderTarget2D* leftPortalRenderTarget;				  // 0x650(0x08)
	struct UTextureRenderTarget2D* rightPortalRenderTarget;				  // 0x658(0x08)
	char pad_660[0x4];													  // 0x660(0x04)
	char PauseMenu[0x08];												  // 0x664(0x08)
	char pad_66C[0x4];													  // 0x66c(0x04)
	struct UPortalWarsPauseMenuWidget* PauseMenuWidgetClass;			  // 0x670(0x08)
	char PauseMenuSceneData[0x50];										  // 0x678(0x50)
	float PredictionFudgeFactor;										  // 0x6c8(0x04)
	float MaxPredictionPing;											  // 0x6cc(0x04)
	float MaxMeleePredictionPing;										  // 0x6d0(0x04)
	char pad_6D4[0x4];													  // 0x6d4(0x04)
	struct TArray<struct FActorOriginAndExtent> actorOriginAndExtentList; // 0x6d8(0x10)
	char pad_6E8[0x30];													  // 0x6e8(0x30)
	struct UPortalWarsKillcam* Killcam;									  // 0x718(0x08)
	char pad_720[0x10];													  // 0x720(0x10)
	struct UMaterial* OutlinePPMaterial;								  // 0x730(0x08)
	char pad_738[0x8];													  // 0x738(0x08)

	void ServerSpawnPickup(struct FName pickupClassName);																												 // Function PortalWars.PortalWarsPlayerController.ServerSpawnPickup // (Net|NetReliableNative|Event|Public|NetServer|NetValidate) // @ game+0x16be0b0
	void ServerSlomo(float TimeDilation);																																 // Function PortalWars.PortalWarsPlayerController.ServerSlomo // (Net|NetReliableNative|Event|Public|NetServer|NetValidate) // @ game+0x16bdff0
	void ServerSetPartyChatOnly(bool bPartyChatOnly);																													 // Function PortalWars.PortalWarsPlayerController.ServerSetPartyChatOnly // (Net|NetReliableNative|Event|Protected|NetServer|NetValidate) // @ game+0x16bdf30
	void ServerSetFOV(float FOV);																																		 // Function PortalWars.PortalWarsPlayerController.ServerSetFOV // (Net|NetReliableNative|Event|Protected|NetServer|NetValidate) // @ game+0x16bde70
	void ServerSetAnonymousMode(bool bAnonymousMode);																													 // Function PortalWars.PortalWarsPlayerController.ServerSetAnonymousMode // (Net|NetReliableNative|Event|Protected|NetServer|NetValidate) // @ game+0x16bddb0
	void ServerRequestReturnToMainMenu(struct FText ReturnReason);																										 // Function PortalWars.PortalWarsPlayerController.ServerRequestReturnToMainMenu // (Net|NetReliableNative|Event|Public|NetServer) // @ game+0x16bdcf0
	void ServerReceiveRanks(struct TArray<struct FUserRankInfo> PlayerRanks);																							 // Function PortalWars.PortalWarsPlayerController.ServerReceiveRanks // (Net|NetReliableNative|Event|Public|NetServer) // @ game+0x16bdc20
	void ServerReceiveProgression(struct FUserProgressionInfo UserProgression);																							 // Function PortalWars.PortalWarsPlayerController.ServerReceiveProgression // (Net|NetReliableNative|Event|Public|NetServer) // @ game+0x16bdb90
	void ServerReceiveDailyStreak(struct FUserDailyStreakInfo DailyStreak);																								 // Function PortalWars.PortalWarsPlayerController.ServerReceiveDailyStreak // (Net|NetReliableNative|Event|Public|NetServer) // @ game+0x16bdaf0
	void ServerReceiveCustomizations(struct FEquippedCustomizations ChosenCustomizations);																				 // Function PortalWars.PortalWarsPlayerController.ServerReceiveCustomizations // (Net|NetReliableNative|Event|Public|NetServer) // @ game+0x16bda20
	void ServerReceiveBadgeProgress(struct FUserBadgeProgressInfo BadgeProgress);																						 // Function PortalWars.PortalWarsPlayerController.ServerReceiveBadgeProgress // (Net|NetReliableNative|Event|Public|NetServer) // @ game+0x16bd950
	void ServerNotifyCinematicFinished();																																 // Function PortalWars.PortalWarsPlayerController.ServerNotifyCinematicFinished // (Net|NetReliableNative|Event|Protected|NetServer) // @ game+0x16bd930
	void ServerKickPlayer(struct APlayerState* PlayerStateToKick);																										 // Function PortalWars.PortalWarsPlayerController.ServerKickPlayer // (Net|NetReliableNative|Event|Public|NetServer|NetValidate) // @ game+0x16bd870
	void ServerEndMatch();																																				 // Function PortalWars.PortalWarsPlayerController.ServerEndMatch // (Net|NetReliableNative|Event|Public|NetServer|NetValidate) // @ game+0x16bd820
	void ServerEnableGodMode();																																			 // Function PortalWars.PortalWarsPlayerController.ServerEnableGodMode // (Net|NetReliableNative|Event|Public|NetServer|NetValidate) // @ game+0x16bd7d0
	void ServerEnableCheats();																																			 // Function PortalWars.PortalWarsPlayerController.ServerEnableCheats // (Net|NetReliableNative|Event|Public|NetServer|NetValidate) // @ game+0x16bd780
	void ServerBroadcastChatMessage(struct FTextChatData InData);																										 // Function PortalWars.PortalWarsPlayerController.ServerBroadcastChatMessage // (Net|Native|Event|Public|NetServer|BlueprintCallable|NetValidate) // @ game+0x16bd410
	void ServerAcknowledgeReadyToStartForge();																															 // Function PortalWars.PortalWarsPlayerController.ServerAcknowledgeReadyToStartForge // (Net|NetReliableNative|Event|Protected|NetServer|NetValidate) // @ game+0x16bd3c0
	void SendRanksToServer();																																			 // Function PortalWars.PortalWarsPlayerController.SendRanksToServer // (Native|Public) // @ game+0x16bd3a0
	void SendCustomizationsToServer();																																	 // Function PortalWars.PortalWarsPlayerController.SendCustomizationsToServer // (Native|Public) // @ game+0x16bd380
	bool ProjectWorldLocationToScreenCustom(struct FVector WorldLocation, struct FVector2D& ScreenLocation, bool bPlayerViewportRelative);								 // Function PortalWars.PortalWarsPlayerController.ProjectWorldLocationToScreenCustom // (Final|Native|Public|HasOutParms|HasDefaults|BlueprintCallable|BlueprintPure|Const) // @ game+0x16bd200
	void PlayEmote();																																					 // Function PortalWars.PortalWarsPlayerController.PlayEmote // (Native|Public) // @ game+0x16bd1e0
	void OnPauseMenuClosed();																																			 // Function PortalWars.PortalWarsPlayerController.OnPauseMenuClosed // (Native|Public) // @ game+0x16bcd60
	void InitOutroCinematic();																																			 // Function PortalWars.PortalWarsPlayerController.InitOutroCinematic // (Final|Native|Public) // @ game+0x16bcc60
	void InitLevelSequencePlayer();																																		 // Function PortalWars.PortalWarsPlayerController.InitLevelSequencePlayer // (Final|Native|Protected) // @ game+0x16bcc40
	void InitIntroCinematic();																																			 // Function PortalWars.PortalWarsPlayerController.InitIntroCinematic // (Final|Native|Public) // @ game+0x16bcc20
	int32_t GetTeamNum();																																				 // Function PortalWars.PortalWarsPlayerController.GetTeamNum // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x16bcb50
	void ClientUpdateChat(struct FTextChatData InData);																													 // Function PortalWars.PortalWarsPlayerController.ClientUpdateChat // (Net|NetReliableNative|Event|Public|NetClient) // @ game+0x16bc5b0
	void ClientSetSpectatorCamera(struct FVector CameraLocation, struct FRotator CameraRotation);																		 // Function PortalWars.PortalWarsPlayerController.ClientSetSpectatorCamera // (Net|NetReliableNative|Event|Public|HasDefaults|NetClient) // @ game+0x16bc390
	void ClientSetRoundResult(int32_t WinningTeamNum, bool bIsTie, int32_t Placement);																					 // Function PortalWars.PortalWarsPlayerController.ClientSetRoundResult // (Net|NetReliableNative|Event|Public|NetClient) // @ game+0x16bc280
	void ClientSetMatchResult(int32_t WinningTeamNum, bool bIsTie, int32_t Placement, bool bPlayFinalKillcam);															 // Function PortalWars.PortalWarsPlayerController.ClientSetMatchResult // (Net|NetReliableNative|Event|Public|NetClient) // @ game+0x16bc120
	void ClientNotifyTeammateTeabagConfirmed();																															 // Function PortalWars.PortalWarsPlayerController.ClientNotifyTeammateTeabagConfirmed // (Net|Native|Event|Public|NetClient) // @ game+0x16bbfd0
	void ClientNotifyTeabagDenied();																																	 // Function PortalWars.PortalWarsPlayerController.ClientNotifyTeabagDenied // (Net|Native|Event|Public|NetClient) // @ game+0x16bbfb0
	void ClientNotifyTeabagConfirmed();																																	 // Function PortalWars.PortalWarsPlayerController.ClientNotifyTeabagConfirmed // (Net|Native|Event|Public|NetClient) // @ game+0x16bbf90
	void ClientNotifyEnemyDeniedTeabag();																																 // Function PortalWars.PortalWarsPlayerController.ClientNotifyEnemyDeniedTeabag // (Net|Native|Event|Public|NetClient) // @ game+0x16bbf70
	void ClientNotifyDamageTaken(struct FVector_NetQuantize HitLocation, bool bThruPortal);																				 // Function PortalWars.PortalWarsPlayerController.ClientNotifyDamageTaken // (Net|Native|Event|Public|NetClient) // @ game+0x16bbe90
	void ClientNotifyCountdown(int32_t CountdownTime);																													 // Function PortalWars.PortalWarsPlayerController.ClientNotifyCountdown // (Net|NetReliableNative|Event|Public|NetClient) // @ game+0x16bbe00
	void ClientNotifyCausedHit(struct APawn* DamagedPawn, float DamageDealt, enum class EPWHitType HitType);															 // Function PortalWars.PortalWarsPlayerController.ClientNotifyCausedHit // (Net|NetReliableNative|Event|Public|NetClient) // @ game+0x16bbcf0
	void ClientNotifyAlive();																																			 // Function PortalWars.PortalWarsPlayerController.ClientNotifyAlive // (Net|NetReliableNative|Event|Public|NetClient) // @ game+0x16bbcd0
	void ClientHearAkEventAtLocation(struct UAkAudioEvent* EventToPost, struct FVector_NetQuantize SoundLocation, struct FRotator SoundRotation, bool bUseSpatialAudio); // Function PortalWars.PortalWarsPlayerController.ClientHearAkEventAtLocation // (Net|Native|Event|Public|HasDefaults|NetClient) // @ game+0x16bbb60
	void ClientHearAkEvent(struct UAkAudioEvent* EventToPost, struct UPortalWarsAkComponent* AkComponent, bool bUseSpatialAudio);										 // Function PortalWars.PortalWarsPlayerController.ClientHearAkEvent // (Net|Native|Event|Public|NetClient) // @ game+0x16bba50
	void ClientGenericInitialization();																																	 // Function PortalWars.PortalWarsPlayerController.ClientGenericInitialization // (Net|NetReliableNative|Event|Public|NetClient) // @ game+0x16bba30
	void ClientGameStarted();																																			 // Function PortalWars.PortalWarsPlayerController.ClientGameStarted // (Net|NetReliableNative|Event|Public|NetClient) // @ game+0x16bba10
	void ClientCountdownStarted();																																		 // Function PortalWars.PortalWarsPlayerController.ClientCountdownStarted // (Net|NetReliableNative|Event|Public|NetClient) // @ game+0x16bb9f0
	void CheatSpawnPickup(struct FName pickupClassName);																												 // Function PortalWars.PortalWarsPlayerController.CheatSpawnPickup // (Final|Exec|Native|Public) // @ game+0x16bb960
	void CheatSlowmo(float TimeDilation);																																 // Function PortalWars.PortalWarsPlayerController.CheatSlowmo // (Final|Exec|Native|Public) // @ game+0x16bb8e0
	void CheatShootThroughWalls();																																		 // Function PortalWars.PortalWarsPlayerController.CheatShootThroughWalls // (Final|Exec|Native|Public) // @ game+0xf085f0
	void CheatRespawn();																																				 // Function PortalWars.PortalWarsPlayerController.CheatRespawn // (Final|Exec|Native|Public) // @ game+0xf085f0
	void CheatGodMode();																																				 // Function PortalWars.PortalWarsPlayerController.CheatGodMode // (Final|Exec|Native|Public) // @ game+0x16bb8c0
	void CheatESP();																																					 // Function PortalWars.PortalWarsPlayerController.CheatESP // (Final|Exec|Native|Public) // @ game+0xf085f0
	void CheatEnableCheats();																																			 // Function PortalWars.PortalWarsPlayerController.CheatEnableCheats // (Final|Exec|Native|Public) // @ game+0x16bb8a0

	void SendChatMessage(FString Message, enum class EChatType ChatType = EChatType::General);
};
