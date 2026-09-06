#pragma once

#include "Fwd.h"
#include "Values.h"
#include "AController.h"

// Class Engine.PlayerController
struct APlayerController : AController
{
	struct UPlayer* Player;														 // 0x298(0x08)
	struct APawn* AcknowledgedPawn;												 // 0x2a0(0x08)
	struct UInterpTrackInstDirector* ControllingDirTrackInst;					 // 0x2a8(0x08)
	struct AHUD* MyHUD;															 // 0x2b0(0x08)
	struct APlayerCameraManager* PlayerCameraManager;							 // 0x2b8(0x08)
	struct APlayerCameraManager* PlayerCameraManagerClass;						 // 0x2c0(0x08)
	bool bAutoManageActiveCameraTarget;											 // 0x2c8(0x01)
	char pad_2C9[0x3];															 // 0x2c9(0x03)
	struct FRotator TargetViewRotation;											 // 0x2cc(0x0c)
	char pad_2D8[0xc];															 // 0x2d8(0x0c)
	float SmoothTargetViewRotationSpeed;										 // 0x2e4(0x04)
	char pad_2E8[0x8];															 // 0x2e8(0x08)
	struct TArray<struct AActor*> HiddenActors;									 // 0x2f0(0x10)
	char HiddenPrimitiveComponents[0x10];										 // 0x300(0x10)
	char pad_310[0x4];															 // 0x310(0x04)
	float LastSpectatorStateSynchTime;											 // 0x314(0x04)
	struct FVector LastSpectatorSyncLocation;									 // 0x318(0x0c)
	struct FRotator LastSpectatorSyncRotation;									 // 0x324(0x0c)
	int32_t ClientCap;															 // 0x330(0x04)
	char pad_334[0x4];															 // 0x334(0x04)
	struct UCheatManager* CheatManager;											 // 0x338(0x08)
	struct UCheatManager* CheatClass;											 // 0x340(0x08)
	struct UPlayerInput* PlayerInput;											 // 0x348(0x08)
	struct TArray<struct FActiveForceFeedbackEffect> ActiveForceFeedbackEffects; // 0x350(0x10)
	char pad_360[0x70];															 // 0x360(0x70)
	char pad_3D0_0 : 4;															 // 0x3d0(0x01)
	char bPlayerIsWaiting : 1;													 // 0x3d0(0x01)
	char pad_3D0_5 : 3;															 // 0x3d0(0x01)
	char pad_3D1[0x3];															 // 0x3d1(0x03)
	char NetPlayerIndex;														 // 0x3d4(0x01)
	char pad_3D5[0x3b];															 // 0x3d5(0x3b)
	struct UNetConnection* PendingSwapConnection;								 // 0x410(0x08)
	struct UNetConnection* NetConnection;										 // 0x418(0x08)
	char pad_420[0xc];															 // 0x420(0x0c)
	float InputYawScale;														 // 0x42c(0x04)
	float InputPitchScale;														 // 0x430(0x04)
	float InputRollScale;														 // 0x434(0x04)
	char bShowMouseCursor : 1;													 // 0x438(0x01)
	char bEnableClickEvents : 1;												 // 0x438(0x01)
	char bEnableTouchEvents : 1;												 // 0x438(0x01)
	char bEnableMouseOverEvents : 1;											 // 0x438(0x01)
	char bEnableTouchOverEvents : 1;											 // 0x438(0x01)
	char bForceFeedbackEnabled : 1;												 // 0x438(0x01)
	char pad_438_6 : 2;															 // 0x438(0x01)
	char pad_439[0x3];															 // 0x439(0x03)
	float ForceFeedbackScale;													 // 0x43c(0x04)
	struct TArray<struct FKey> ClickEventKeys;									 // 0x440(0x10)
	enum class EMouseCursor DefaultMouseCursor;									 // 0x450(0x01)
	enum class EMouseCursor CurrentMouseCursor;									 // 0x451(0x01)
	char DefaultClickTraceChannel[0x01];										 // 0x452(0x01)
	char CurrentClickTraceChannel[0x01];										 // 0x453(0x01)
	float HitResultTraceDistance;												 // 0x454(0x04)
	uint16_t SeamlessTravelCount;												 // 0x458(0x02)
	uint16_t LastCompletedSeamlessTravelCount;									 // 0x45a(0x02)
	char pad_45C[0x74];															 // 0x45c(0x74)
	struct UInputComponent* InactiveStateInputComponent;						 // 0x4d0(0x08)
	char pad_4D8_0 : 2;															 // 0x4d8(0x01)
	char bShouldPerformFullTickWhenPaused : 1;									 // 0x4d8(0x01)
	char pad_4D8_3 : 5;															 // 0x4d8(0x01)
	char pad_4D9[0x17];															 // 0x4d9(0x17)
	struct UTouchInterface* CurrentTouchInterface;								 // 0x4f0(0x08)
	char pad_4F8[0x50];															 // 0x4f8(0x50)
	struct ASpectatorPawn* SpectatorPawn;										 // 0x548(0x08)
	char pad_550[0x4];															 // 0x550(0x04)
	bool bIsLocalPlayerController;												 // 0x554(0x01)
	char pad_555[0x3];															 // 0x555(0x03)
	struct FVector SpawnLocation;												 // 0x558(0x0c)
	char pad_564[0xc];															 // 0x564(0x0c)

	bool WasInputKeyJustReleased(struct FKey Key);																																																		  // Function Engine.PlayerController.WasInputKeyJustReleased // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x37b6930
	bool WasInputKeyJustPressed(struct FKey Key);																																																		  // Function Engine.PlayerController.WasInputKeyJustPressed // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x37b6840
	void ToggleSpeaking(bool bInSpeaking);																																																				  // Function Engine.PlayerController.ToggleSpeaking // (Exec|Native|Public) // @ game+0x37b67b0
	void TestServerLevelVisibilityChange(struct FName PackageName, struct FName Filename);																																								  // Function Engine.PlayerController.TestServerLevelVisibilityChange // (Final|Exec|Native|Private) // @ game+0x37b66e0
	void SwitchLevel(struct FString URL);																																																				  // Function Engine.PlayerController.SwitchLevel // (Exec|Native|Public) // @ game+0x37b6640
	void StopHapticEffect(enum class EControllerHand Hand);																																																  // Function Engine.PlayerController.StopHapticEffect // (Final|Native|Public|BlueprintCallable) // @ game+0x37b65a0
	void StartFire(char FireModeNum);																																																					  // Function Engine.PlayerController.StartFire // (Exec|Native|Public) // @ game+0x37b5ec0
	void SetVirtualJoystickVisibility(bool bVisible);																																																	  // Function Engine.PlayerController.SetVirtualJoystickVisibility // (Native|Public|BlueprintCallable) // @ game+0x37b58d0
	void SetViewTargetWithBlend(struct AActor* NewViewTarget, float BlendTime, enum class EViewTargetBlendFunction BlendFunc, float BlendExp, bool bLockOutgoing);																						  // Function Engine.PlayerController.SetViewTargetWithBlend // (Native|Public|BlueprintCallable) // @ game+0x37b5730
	void SetName(struct FString S);																																																						  // Function Engine.PlayerController.SetName // (Exec|Native|Public) // @ game+0x37b5690
	void SetMouseLocation(int32_t X, int32_t Y);																																																		  // Function Engine.PlayerController.SetMouseLocation // (Final|Native|Public|BlueprintCallable) // @ game+0x37b55c0
	void SetMouseCursorWidget(enum class EMouseCursor Cursor, struct UUserWidget* CursorWidget);																																						  // Function Engine.PlayerController.SetMouseCursorWidget // (Final|Native|Public|BlueprintCallable) // @ game+0x37b54f0
	void SetHapticsByValue(float Frequency, float Amplitude, enum class EControllerHand Hand);																																							  // Function Engine.PlayerController.SetHapticsByValue // (Final|Native|Public|BlueprintCallable) // @ game+0x37b52b0
	void SetDisableHaptics(bool bNewDisabled);																																																			  // Function Engine.PlayerController.SetDisableHaptics // (Native|Public|BlueprintCallable) // @ game+0x37b5200
	void SetControllerLightColor(struct FColor Color);																																																	  // Function Engine.PlayerController.SetControllerLightColor // (Final|Native|Public|HasDefaults|BlueprintCallable) // @ game+0x37b5180
	void SetCinematicMode(bool bInCinematicMode, bool bHidePlayer, bool bAffectsHUD, bool bAffectsMovement, bool bAffectsTurning);																														  // Function Engine.PlayerController.SetCinematicMode // (Native|Public|BlueprintCallable) // @ game+0x37b4fa0
	void SetAudioListenerOverride(struct USceneComponent* AttachToComponent, struct FVector Location, struct FRotator Rotation);																														  // Function Engine.PlayerController.SetAudioListenerOverride // (Final|Native|Public|HasDefaults|BlueprintCallable) // @ game+0x37b4e70
	void SetAudioListenerAttenuationOverride(struct USceneComponent* AttachToComponent, struct FVector AttenuationLocationOVerride);																													  // Function Engine.PlayerController.SetAudioListenerAttenuationOverride // (Final|Native|Public|HasDefaults|BlueprintCallable) // @ game+0x37b4d90
	void ServerViewSelf(struct FViewTargetTransitionParams TransitionParams);																																											  // Function Engine.PlayerController.ServerViewSelf // (Net|Native|Event|Public|NetServer|NetValidate) // @ game+0x37b4ca0
	void ServerViewPrevPlayer();																																																						  // Function Engine.PlayerController.ServerViewPrevPlayer // (Net|Native|Event|Public|NetServer|NetValidate) // @ game+0x37b4c50
	void ServerViewNextPlayer();																																																						  // Function Engine.PlayerController.ServerViewNextPlayer // (Net|Native|Event|Public|NetServer|NetValidate) // @ game+0x37b4c00
	void ServerVerifyViewTarget();																																																						  // Function Engine.PlayerController.ServerVerifyViewTarget // (Net|NetReliableNative|Event|Public|NetServer|NetValidate) // @ game+0x37b4bb0
	void ServerUpdateMultipleLevelsVisibility(struct TArray<struct FUpdateLevelVisibilityLevelInfo> LevelVisibilities);																																	  // Function Engine.PlayerController.ServerUpdateMultipleLevelsVisibility // (Final|Net|NetReliableNative|Event|Public|NetServer|NetValidate) // @ game+0x37b4af0
	void ServerUpdateLevelVisibility(struct FUpdateLevelVisibilityLevelInfo LevelVisibility);																																							  // Function Engine.PlayerController.ServerUpdateLevelVisibility // (Final|Net|NetReliableNative|Event|Public|NetServer|NetValidate) // @ game+0x37b4a30
	void ServerUpdateCamera(struct FVector_NetQuantize CamLoc, int32_t CamPitchAndYaw);																																									  // Function Engine.PlayerController.ServerUpdateCamera // (Net|Native|Event|Public|NetServer|NetValidate) // @ game+0x37b4900
	void ServerUnmutePlayer(struct FUniqueNetIdRepl PlayerId);																																															  // Function Engine.PlayerController.ServerUnmutePlayer // (Net|NetReliableNative|Event|Public|NetServer|NetValidate) // @ game+0x37b47a0
	void ServerToggleAILogging();																																																						  // Function Engine.PlayerController.ServerToggleAILogging // (Net|NetReliableNative|Event|Public|NetServer|NetValidate) // @ game+0x37b4750
	void ServerShortTimeout();																																																							  // Function Engine.PlayerController.ServerShortTimeout // (Net|NetReliableNative|Event|Public|NetServer|NetValidate) // @ game+0x37b4700
	void ServerSetSpectatorWaiting(bool bWaiting);																																																		  // Function Engine.PlayerController.ServerSetSpectatorWaiting // (Net|NetReliableNative|Event|Public|NetServer|NetValidate) // @ game+0x37b4640
	void ServerSetSpectatorLocation(struct FVector NewLoc, struct FRotator NewRot);																																										  // Function Engine.PlayerController.ServerSetSpectatorLocation // (Net|Native|Event|Public|NetServer|HasDefaults|NetValidate) // @ game+0x37b4500
	void ServerRestartPlayer();																																																							  // Function Engine.PlayerController.ServerRestartPlayer // (Net|NetReliableNative|Event|Public|NetServer|NetValidate) // @ game+0x37b44b0
	void ServerPause();																																																									  // Function Engine.PlayerController.ServerPause // (Net|NetReliableNative|Event|Public|NetServer|NetValidate) // @ game+0x37b4460
	void ServerNotifyLoadedWorld(struct FName WorldPackageName);																																														  // Function Engine.PlayerController.ServerNotifyLoadedWorld // (Net|NetReliableNative|Event|Public|NetServer|NetValidate) // @ game+0x37b43a0
	void ServerMutePlayer(struct FUniqueNetIdRepl PlayerId);																																															  // Function Engine.PlayerController.ServerMutePlayer // (Net|NetReliableNative|Event|Public|NetServer|NetValidate) // @ game+0x37b4240
	void ServerExecRPC(struct FString Msg);																																																				  // Function Engine.PlayerController.ServerExecRPC // (Net|NetReliableNative|Event|Public|NetServer|NetValidate) // @ game+0x37b4180
	void ServerExec(struct FString Msg);																																																				  // Function Engine.PlayerController.ServerExec // (Final|Exec|Native|Public) // @ game+0x37b40e0
	void ServerCheckClientPossessionReliable();																																																			  // Function Engine.PlayerController.ServerCheckClientPossessionReliable // (Net|NetReliableNative|Event|Public|NetServer|NetValidate) // @ game+0x37b4090
	void ServerCheckClientPossession();																																																					  // Function Engine.PlayerController.ServerCheckClientPossession // (Net|Native|Event|Public|NetServer|NetValidate) // @ game+0x37b4040
	void ServerChangeName(struct FString S);																																																			  // Function Engine.PlayerController.ServerChangeName // (Net|NetReliableNative|Event|Public|NetServer|NetValidate) // @ game+0x37b3f80
	void ServerCamera(struct FName NewMode);																																																			  // Function Engine.PlayerController.ServerCamera // (Net|NetReliableNative|Event|Public|NetServer|NetValidate) // @ game+0x37b3ec0
	void ServerAcknowledgePossession(struct APawn* P);																																																	  // Function Engine.PlayerController.ServerAcknowledgePossession // (Net|NetReliableNative|Event|Public|NetServer|NetValidate) // @ game+0x37b3e00
	void SendToConsole(struct FString Command);																																																			  // Function Engine.PlayerController.SendToConsole // (Exec|Native|Public) // @ game+0x37b3d60
	void RestartLevel();																																																								  // Function Engine.PlayerController.RestartLevel // (Exec|Native|Public) // @ game+0x37b3d40
	void ResetControllerLightColor();																																																					  // Function Engine.PlayerController.ResetControllerLightColor // (Final|Native|Public|BlueprintCallable) // @ game+0x37b3d20
	bool ProjectWorldLocationToScreen(struct FVector WorldLocation, struct FVector2D& ScreenLocation, bool bPlayerViewportRelative);																													  // Function Engine.PlayerController.ProjectWorldLocationToScreen // (Final|Native|Public|HasOutParms|HasDefaults|BlueprintCallable|BlueprintPure|Const) // @ game+0x37b3b40
	void PlayHapticEffect(struct UHapticFeedbackEffect_Base* HapticEffect, enum class EControllerHand Hand, float Scale, bool bLoop);																													  // Function Engine.PlayerController.PlayHapticEffect // (Final|Native|Public|BlueprintCallable) // @ game+0x37b39e0
	void PlayDynamicForceFeedback(float Intensity, float Duration, bool bAffectsLeftLarge, bool bAffectsLeftSmall, bool bAffectsRightLarge, bool bAffectsRightSmall, enum class EDynamicForceFeedbackAction Action, struct FLatentActionInfo LatentInfo); // Function Engine.PlayerController.PlayDynamicForceFeedback // (Final|Native|Private|BlueprintCallable) // @ game+0x37b3730
	void Pause();																																																										  // Function Engine.PlayerController.Pause // (Exec|Native|Public) // @ game+0x37b3270
	void OnServerStartedVisualLogger(bool bIsLogging);																																																	  // Function Engine.PlayerController.OnServerStartedVisualLogger // (Net|NetReliableNative|Event|Public|NetClient) // @ game+0x37b31e0
	void LocalTravel(struct FString URL);																																																				  // Function Engine.PlayerController.LocalTravel // (Exec|Native|Public) // @ game+0x37b3140
	void K2_ClientPlayForceFeedback(struct UForceFeedbackEffect* ForceFeedbackEffect, struct FName Tag, bool bLooping, bool bIgnoreTimeDilation, bool bPlayWhilePaused);																				  // Function Engine.PlayerController.K2_ClientPlayForceFeedback // (Final|Native|Public|BlueprintCallable) // @ game+0x37b2f80
	bool IsInputKeyDown(struct FKey Key);																																																				  // Function Engine.PlayerController.IsInputKeyDown // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x37b2e90
	void GetViewportSize(int32_t& SizeX, int32_t& SizeY);																																																  // Function Engine.PlayerController.GetViewportSize // (Final|Native|Public|HasOutParms|BlueprintCallable|BlueprintPure|Const) // @ game+0x37b2db0
	struct ASpectatorPawn* GetSpectatorPawn();																																																			  // Function Engine.PlayerController.GetSpectatorPawn // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x37b2d90
	bool GetMousePosition(float& LocationX, float& LocationY);																																															  // Function Engine.PlayerController.GetMousePosition // (Final|Native|Public|HasOutParms|BlueprintCallable|BlueprintPure|Const) // @ game+0x37b2c70
	struct FVector GetInputVectorKeyState(struct FKey Key);																																																  // Function Engine.PlayerController.GetInputVectorKeyState // (Final|Native|Public|HasDefaults|BlueprintCallable|BlueprintPure|Const) // @ game+0x37b2b60
	void GetInputTouchState(enum class ETouchIndex FingerIndex, float& LocationX, float& LocationY, bool& bIsCurrentlyPressed);																															  // Function Engine.PlayerController.GetInputTouchState // (Final|Native|Public|HasOutParms|BlueprintCallable|BlueprintPure|Const) // @ game+0x37b29d0
	void GetInputMouseDelta(float& DeltaX, float& DeltaY);																																																  // Function Engine.PlayerController.GetInputMouseDelta // (Final|Native|Public|HasOutParms|BlueprintCallable|BlueprintPure|Const) // @ game+0x37b28f0
	void GetInputMotionState(struct FVector& Tilt, struct FVector& RotationRate, struct FVector& Gravity, struct FVector& Acceleration);																												  // Function Engine.PlayerController.GetInputMotionState // (Final|Native|Public|HasOutParms|HasDefaults|BlueprintCallable|BlueprintPure|Const) // @ game+0x37b2760
	float GetInputKeyTimeDown(struct FKey Key);																																																			  // Function Engine.PlayerController.GetInputKeyTimeDown // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x37b2660
	void GetInputAnalogStickState(enum class EControllerAnalogStick WhichStick, float& StickX, float& StickY);																																			  // Function Engine.PlayerController.GetInputAnalogStickState // (Final|Native|Public|HasOutParms|BlueprintCallable|BlueprintPure|Const) // @ game+0x37b2530
	float GetInputAnalogKeyState(struct FKey Key);																																																		  // Function Engine.PlayerController.GetInputAnalogKeyState // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x37b2430
	struct AHUD* GetHUD();																																																								  // Function Engine.PlayerController.GetHUD // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x37b1a20
	bool GetHitResultUnderFingerForObjects(enum class ETouchIndex FingerIndex, struct TArray<enum class EObjectTypeQuery>& ObjectTypes, bool bTraceComplex, struct FHitResult& HitResult);																  // Function Engine.PlayerController.GetHitResultUnderFingerForObjects // (Final|Native|Public|HasOutParms|BlueprintCallable|BlueprintPure|Const) // @ game+0x37b2260
	bool GetHitResultUnderFingerByChannel(enum class ETouchIndex FingerIndex, enum class ETraceTypeQuery TraceChannel, bool bTraceComplex, struct FHitResult& HitResult);																				  // Function Engine.PlayerController.GetHitResultUnderFingerByChannel // (Final|Native|Public|HasOutParms|BlueprintCallable|BlueprintPure|Const) // @ game+0x37b20b0
	bool GetHitResultUnderFinger(enum class ETouchIndex FingerIndex, enum class ECollisionChannel TraceChannel, bool bTraceComplex, struct FHitResult& HitResult);																						  // Function Engine.PlayerController.GetHitResultUnderFinger // (Final|Native|Public|HasOutParms|BlueprintCallable|BlueprintPure|Const) // @ game+0x37b1f00
	bool GetHitResultUnderCursorForObjects(struct TArray<enum class EObjectTypeQuery>& ObjectTypes, bool bTraceComplex, struct FHitResult& HitResult);																									  // Function Engine.PlayerController.GetHitResultUnderCursorForObjects // (Final|Native|Public|HasOutParms|BlueprintCallable|BlueprintPure|Const) // @ game+0x37b1d70
	bool GetHitResultUnderCursorByChannel(enum class ETraceTypeQuery TraceChannel, bool bTraceComplex, struct FHitResult& HitResult);																													  // Function Engine.PlayerController.GetHitResultUnderCursorByChannel // (Final|Native|Public|HasOutParms|BlueprintCallable|BlueprintPure|Const) // @ game+0x37b1be0
	bool GetHitResultUnderCursor(enum class ECollisionChannel TraceChannel, bool bTraceComplex, struct FHitResult& HitResult);																															  // Function Engine.PlayerController.GetHitResultUnderCursor // (Final|Native|Public|HasOutParms|BlueprintCallable|BlueprintPure|Const) // @ game+0x37b1a50
	struct FVector GetFocalLocation();																																																					  // Function Engine.PlayerController.GetFocalLocation // (Native|Public|HasDefaults|BlueprintCallable|BlueprintPure|Const) // @ game+0x37b19e0
	void FOV(float NewFOV);																																																								  // Function Engine.PlayerController.FOV // (Exec|Native|Public) // @ game+0x37b1830
	void EnableCheats();																																																								  // Function Engine.PlayerController.EnableCheats // (Exec|Native|Public) // @ game+0x37b1810
	bool DeprojectScreenPositionToWorld(float ScreenX, float ScreenY, struct FVector& WorldLocation, struct FVector& WorldDirection);																													  // Function Engine.PlayerController.DeprojectScreenPositionToWorld // (Final|Native|Public|HasOutParms|HasDefaults|BlueprintCallable|BlueprintPure|Const) // @ game+0x37b1680
	bool DeprojectMousePositionToWorld(struct FVector& WorldLocation, struct FVector& WorldDirection);																																					  // Function Engine.PlayerController.DeprojectMousePositionToWorld // (Final|Native|Public|HasOutParms|HasDefaults|BlueprintCallable|BlueprintPure|Const) // @ game+0x37b1580
	void ConsoleKey(struct FKey Key);																																																					  // Function Engine.PlayerController.ConsoleKey // (Exec|Native|Public) // @ game+0x37b1490
	void ClientWasKicked(struct FText KickReason);																																																		  // Function Engine.PlayerController.ClientWasKicked // (Net|NetReliableNative|Event|Public|NetClient) // @ game+0x37b13d0
	void ClientVoiceHandshakeComplete();																																																				  // Function Engine.PlayerController.ClientVoiceHandshakeComplete // (Net|NetReliableNative|Event|Public|NetClient) // @ game+0x37b13b0
	void ClientUpdateMultipleLevelsStreamingStatus(struct TArray<struct FUpdateLevelStreamingLevelStatus> LevelStatuses);																																  // Function Engine.PlayerController.ClientUpdateMultipleLevelsStreamingStatus // (Net|NetReliableNative|Event|Public|NetClient) // @ game+0x37b1310
	void ClientUpdateLevelStreamingStatus(struct FName PackageName, bool bNewShouldBeLoaded, bool bNewShouldBeVisible, bool bNewShouldBlockOnLoad, int32_t LODIndex);																					  // Function Engine.PlayerController.ClientUpdateLevelStreamingStatus // (Net|NetReliableNative|Event|Public|NetClient) // @ game+0x37b1150
	void ClientUnmutePlayer(struct FUniqueNetIdRepl PlayerId);																																															  // Function Engine.PlayerController.ClientUnmutePlayer // (Net|NetReliableNative|Event|Public|NetClient) // @ game+0x37b1030
	void ClientTravelInternal(struct FString URL, enum class ETravelType TravelType, bool bSeamless, struct FGuid MapPackageGuid);																														  // Function Engine.PlayerController.ClientTravelInternal // (Net|NetReliableNative|Event|Public|HasDefaults|NetClient) // @ game+0x37b0eb0
	void ClientTravel(struct FString URL, enum class ETravelType TravelType, bool bSeamless, struct FGuid MapPackageGuid);																																  // Function Engine.PlayerController.ClientTravel // (Final|Native|Public|HasDefaults) // @ game+0x37b0d30
	void ClientTeamMessage(struct APlayerState* SenderPlayerState, struct FString S, struct FName Type, float MsgLifeTime);																																  // Function Engine.PlayerController.ClientTeamMessage // (Net|NetReliableNative|Event|Public|NetClient) // @ game+0x37b0bc0
	void ClientStopForceFeedback(struct UForceFeedbackEffect* ForceFeedbackEffect, struct FName Tag);																																					  // Function Engine.PlayerController.ClientStopForceFeedback // (Net|NetReliableNative|Event|Public|NetClient|BlueprintCallable) // @ game+0x37b0af0
	void ClientStopCameraShakesFromSource(struct UCameraShakeSourceComponent* SourceComponent, bool bImmediately);																																		  // Function Engine.PlayerController.ClientStopCameraShakesFromSource // (Final|Native|Public|BlueprintCallable) // @ game+0x37b0a20
	void ClientStopCameraShake(struct UCameraShakeBase* Shake, bool bImmediately);																																										  // Function Engine.PlayerController.ClientStopCameraShake // (Net|NetReliableNative|Event|Public|NetClient|BlueprintCallable) // @ game+0x37b0950
	void ClientStopCameraAnim(struct UCameraAnim* AnimToStop);																																															  // Function Engine.PlayerController.ClientStopCameraAnim // (Net|NetReliableNative|Event|Public|NetClient) // @ game+0x37b08c0
	void ClientStartOnlineSession();																																																					  // Function Engine.PlayerController.ClientStartOnlineSession // (Net|NetReliableNative|Event|Public|NetClient) // @ game+0x37b08a0
	void ClientStartCameraShakeFromSource(struct UCameraShakeBase* Shake, struct UCameraShakeSourceComponent* SourceComponent);																															  // Function Engine.PlayerController.ClientStartCameraShakeFromSource // (Final|Native|Public|BlueprintCallable) // @ game+0x37b07d0
	void ClientStartCameraShake(struct UCameraShakeBase* Shake, float Scale, enum class ECameraShakePlaySpace PlaySpace, struct FRotator UserPlaySpaceRot);																								  // Function Engine.PlayerController.ClientStartCameraShake // (Net|Native|Event|Public|HasDefaults|NetClient|BlueprintCallable) // @ game+0x37b0660
	void ClientSpawnCameraLensEffect(struct AEmitterCameraLensEffectBase* LensEffectEmitterClass);																																						  // Function Engine.PlayerController.ClientSpawnCameraLensEffect // (Net|Native|Event|Public|NetClient|BlueprintCallable) // @ game+0x37b05d0
	void ClientSetViewTarget(struct AActor* A, struct FViewTargetTransitionParams TransitionParams);																																					  // Function Engine.PlayerController.ClientSetViewTarget // (Net|NetReliableNative|Event|Public|NetClient) // @ game+0x37b04d0
	void ClientSetSpectatorWaiting(bool bWaiting);																																																		  // Function Engine.PlayerController.ClientSetSpectatorWaiting // (Net|NetReliableNative|Event|Public|NetClient) // @ game+0x37b0440
	void ClientSetHUD(struct AHUD* NewHUDClass);																																																		  // Function Engine.PlayerController.ClientSetHUD // (Net|NetReliableNative|Event|Public|NetClient|BlueprintCallable) // @ game+0x37b03b0
	void ClientSetForceMipLevelsToBeResident(struct UMaterialInterface* Material, float ForceDuration, int32_t CinematicTextureGroups);																													  // Function Engine.PlayerController.ClientSetForceMipLevelsToBeResident // (Net|NetReliableNative|Event|Public|NetClient) // @ game+0x37b02a0
	void ClientSetCinematicMode(bool bInCinematicMode, bool bAffectsMovement, bool bAffectsTurning, bool bAffectsHUD);																																	  // Function Engine.PlayerController.ClientSetCinematicMode // (Net|NetReliableNative|Event|Public|NetClient) // @ game+0x37b0120
	void ClientSetCameraMode(struct FName NewCamMode);																																																	  // Function Engine.PlayerController.ClientSetCameraMode // (Net|NetReliableNative|Event|Public|NetClient) // @ game+0x37b0090
	void ClientSetCameraFade(bool bEnableFading, struct FColor FadeColor, struct FVector2D FadeAlpha, float FadeTime, bool bFadeAudio, bool bHoldWhenFinished);																							  // Function Engine.PlayerController.ClientSetCameraFade // (Net|NetReliableNative|Event|Public|HasDefaults|NetClient) // @ game+0x37afe80
	void ClientSetBlockOnAsyncLoading();																																																				  // Function Engine.PlayerController.ClientSetBlockOnAsyncLoading // (Net|NetReliableNative|Event|Public|NetClient) // @ game+0x16697c0
	void ClientReturnToMainMenuWithTextReason(struct FText ReturnReason);																																												  // Function Engine.PlayerController.ClientReturnToMainMenuWithTextReason // (Net|NetReliableNative|Event|Public|NetClient) // @ game+0x37afdc0
	void ClientReturnToMainMenu(struct FString ReturnReason);																																															  // Function Engine.PlayerController.ClientReturnToMainMenu // (Net|NetReliableNative|Event|Public|NetClient) // @ game+0x37afd20
	void ClientRetryClientRestart(struct APawn* NewPawn);																																																  // Function Engine.PlayerController.ClientRetryClientRestart // (Net|NetReliableNative|Event|Public|NetClient) // @ game+0x37afc90
	void ClientRestart(struct APawn* NewPawn);																																																			  // Function Engine.PlayerController.ClientRestart // (Net|NetReliableNative|Event|Public|NetClient) // @ game+0x37afc00
	void ClientReset();																																																									  // Function Engine.PlayerController.ClientReset // (Net|NetReliableNative|Event|Public|NetClient) // @ game+0x37afbe0
	void ClientRepObjRef(struct UObject* Object);																																																		  // Function Engine.PlayerController.ClientRepObjRef // (Net|NetReliableNative|Event|Public|NetClient) // @ game+0x37afb50
	void ClientReceiveLocalizedMessage(struct ULocalMessage* Message, int32_t SWITCH, struct APlayerState* RelatedPlayerState_2, struct APlayerState* RelatedPlayerState_3, struct UObject* OptionalObject);											  // Function Engine.PlayerController.ClientReceiveLocalizedMessage // (Net|NetReliableNative|Event|Public|NetClient) // @ game+0x37af9b0
	void ClientPrestreamTextures(struct AActor* ForcedActor, float ForceDuration, bool bEnableStreaming, int32_t CinematicTextureGroups);																												  // Function Engine.PlayerController.ClientPrestreamTextures // (Net|NetReliableNative|Event|Public|NetClient) // @ game+0x37af850
	void ClientPrepareMapChange(struct FName LevelName, bool bFirst, bool bLast);																																										  // Function Engine.PlayerController.ClientPrepareMapChange // (Net|NetReliableNative|Event|Public|NetClient) // @ game+0x37af730
	void ClientPlaySoundAtLocation(struct USoundBase* Sound, struct FVector Location, float VolumeMultiplier, float PitchMultiplier);																													  // Function Engine.PlayerController.ClientPlaySoundAtLocation // (Net|Native|Event|Public|HasDefaults|NetClient) // @ game+0x37af5c0
	void ClientPlaySound(struct USoundBase* Sound, float VolumeMultiplier, float PitchMultiplier);																																						  // Function Engine.PlayerController.ClientPlaySound // (Net|Native|Event|Public|NetClient) // @ game+0x37af4b0
	void ClientPlayForceFeedback_Internal(struct UForceFeedbackEffect* ForceFeedbackEffect, struct FForceFeedbackParameters Params);																													  // Function Engine.PlayerController.ClientPlayForceFeedback_Internal // (Final|Net|Native|Event|Private|NetClient) // @ game+0x37af3c0
	void ClientPlayCameraAnim(struct UCameraAnim* AnimToPlay, float Scale, float Rate, float BlendInTime, float BlendOutTime, bool bLoop, bool bRandomStartTime, enum class ECameraShakePlaySpace Space, struct FRotator CustomPlaySpace);				  // Function Engine.PlayerController.ClientPlayCameraAnim // (Net|Native|Event|Public|HasDefaults|NetClient|BlueprintCallable) // @ game+0x37af0f0
	void ClientMutePlayer(struct FUniqueNetIdRepl PlayerId);																																															  // Function Engine.PlayerController.ClientMutePlayer // (Net|NetReliableNative|Event|Public|NetClient) // @ game+0x37aefd0
	void ClientMessage(struct FString S, struct FName Type, float MsgLifeTime);																																											  // Function Engine.PlayerController.ClientMessage // (Net|NetReliableNative|Event|Public|NetClient) // @ game+0x37aeea0
	void ClientIgnoreMoveInput(bool bIgnore);																																																			  // Function Engine.PlayerController.ClientIgnoreMoveInput // (Net|NetReliableNative|Event|Public|NetClient) // @ game+0x37aee10
	void ClientIgnoreLookInput(bool bIgnore);																																																			  // Function Engine.PlayerController.ClientIgnoreLookInput // (Net|NetReliableNative|Event|Public|NetClient) // @ game+0x37aed80
	void ClientGotoState(struct FName NewState);																																																		  // Function Engine.PlayerController.ClientGotoState // (Net|NetReliableNative|Event|Public|NetClient) // @ game+0x37aecf0
	void ClientGameEnded(struct AActor* EndGameFocus, bool bIsWinner);																																													  // Function Engine.PlayerController.ClientGameEnded // (Net|NetReliableNative|Event|Public|NetClient) // @ game+0x37aec20
	void ClientForceGarbageCollection();																																																				  // Function Engine.PlayerController.ClientForceGarbageCollection // (Net|NetReliableNative|Event|Public|NetClient) // @ game+0x37aec00
	void ClientFlushLevelStreaming();																																																					  // Function Engine.PlayerController.ClientFlushLevelStreaming // (Final|Net|NetReliableNative|Event|Public|NetClient) // @ game+0x37aebe0
	void ClientEndOnlineSession();																																																						  // Function Engine.PlayerController.ClientEndOnlineSession // (Net|NetReliableNative|Event|Public|NetClient) // @ game+0x16592d0
	void ClientEnableNetworkVoice(bool bEnable);																																																		  // Function Engine.PlayerController.ClientEnableNetworkVoice // (Net|NetReliableNative|Event|Public|NetClient) // @ game+0x37aeb50
	void ClientCommitMapChange();																																																						  // Function Engine.PlayerController.ClientCommitMapChange // (Net|NetReliableNative|Event|Public|NetClient) // @ game+0x37aeb30
	void ClientClearCameraLensEffects();																																																				  // Function Engine.PlayerController.ClientClearCameraLensEffects // (Net|NetReliableNative|Event|Public|NetClient|BlueprintCallable) // @ game+0x37aeb10
	void ClientCapBandwidth(int32_t Cap);																																																				  // Function Engine.PlayerController.ClientCapBandwidth // (Net|NetReliableNative|Event|Public|NetClient) // @ game+0x37aea80
	void ClientCancelPendingMapChange();																																																				  // Function Engine.PlayerController.ClientCancelPendingMapChange // (Net|NetReliableNative|Event|Public|NetClient) // @ game+0x37aea60
	void ClientAddTextureStreamingLoc(struct FVector InLoc, float Duration, bool bOverrideLocation);																																					  // Function Engine.PlayerController.ClientAddTextureStreamingLoc // (Final|Net|NetReliableNative|Event|Public|HasDefaults|NetClient) // @ game+0x37ae930
	void ClearAudioListenerOverride();																																																					  // Function Engine.PlayerController.ClearAudioListenerOverride // (Final|Native|Public|BlueprintCallable) // @ game+0x37ae910
	void ClearAudioListenerAttenuationOverride();																																																		  // Function Engine.PlayerController.ClearAudioListenerAttenuationOverride // (Final|Native|Public|BlueprintCallable) // @ game+0x37ae8f0
	bool CanRestartPlayer();																																																							  // Function Engine.PlayerController.CanRestartPlayer // (Native|Public|BlueprintCallable) // @ game+0x37ae8c0
	void Camera(struct FName NewMode);																																																					  // Function Engine.PlayerController.Camera // (Exec|Native|Public) // @ game+0x37ae830
	void AddYawInput(float Val);																																																						  // Function Engine.PlayerController.AddYawInput // (Native|Public|BlueprintCallable) // @ game+0x37ae7a0
	void AddRollInput(float Val);																																																						  // Function Engine.PlayerController.AddRollInput // (Native|Public|BlueprintCallable) // @ game+0x37ae710
	void AddPitchInput(float Val);																																																						  // Function Engine.PlayerController.AddPitchInput // (Native|Public|BlueprintCallable) // @ game+0x37ae680
	void ActivateTouchInterface(struct UTouchInterface* NewTouchInterface);																																												  // Function Engine.PlayerController.ActivateTouchInterface // (Native|Public|BlueprintCallable) // @ game+0x37ae4b0

	bool IsInGame();
};
