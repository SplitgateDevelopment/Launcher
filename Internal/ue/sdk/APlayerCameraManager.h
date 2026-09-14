#pragma once

#include "Fwd.h"
#include "Values.h"
#include "AActor.h"

// Class Engine.PlayerCameraManager
// Size: 0x2810 (Inherited: 0x220)
struct APlayerCameraManager : AActor
{
	struct APlayerController* PCOwner;									   // 0x220(0x08)
	struct USceneComponent* TransformComponent;							   // 0x228(0x08)
	char pad_230[0x8];													   // 0x230(0x08)
	float DefaultFOV;													   // 0x238(0x04)
	char pad_23C[0x4];													   // 0x23c(0x04)
	float DefaultOrthoWidth;											   // 0x240(0x04)
	char pad_244[0x4];													   // 0x244(0x04)
	float DefaultAspectRatio;											   // 0x248(0x04)
	char pad_24C[0x44];													   // 0x24c(0x44)
	struct FCameraCacheEntry CameraCache;								   // 0x290(0x600)
	struct FCameraCacheEntry LastFrameCameraCache;						   // 0x890(0x600)
	struct FTViewTarget ViewTarget;										   // 0xe90(0x610)
	struct FTViewTarget PendingViewTarget;								   // 0x14a0(0x610)
	char pad_1AB0[0x30];												   // 0x1ab0(0x30)
	struct FCameraCacheEntry CameraCachePrivate;						   // 0x1ae0(0x600)
	struct FCameraCacheEntry LastFrameCameraCachePrivate;				   // 0x20e0(0x600)
	struct TArray<struct UCameraModifier*> ModifierList;				   // 0x26e0(0x10)
	struct TArray<struct UCameraModifier*> DefaultModifiers;			   // 0x26f0(0x10)
	float FreeCamDistance;												   // 0x2700(0x04)
	struct FVector FreeCamOffset;										   // 0x2704(0x0c)
	struct FVector ViewTargetOffset;									   // 0x2710(0x0c)
	char pad_271C[0x4];													   // 0x271c(0x04)
	char OnAudioFadeChangeEvent[0x10];									   // 0x2720(0x10)
	char pad_2730[0x10];												   // 0x2730(0x10)
	struct TArray<struct AEmitterCameraLensEffectBase*> CameraLensEffects; // 0x2740(0x10)
	struct UCameraModifier_CameraShake* CachedCameraShakeMod;			   // 0x2750(0x08)
	struct UCameraAnimInst* AnimInstPool[0x8];							   // 0x2758(0x40)
	struct TArray<struct FPostProcessSettings> PostProcessBlendCache;	   // 0x2798(0x10)
	char pad_27A8[0x10];												   // 0x27a8(0x10)
	struct TArray<struct UCameraAnimInst*> ActiveAnims;					   // 0x27b8(0x10)
	struct TArray<struct UCameraAnimInst*> FreeAnims;					   // 0x27c8(0x10)
	struct ACameraActor* AnimCameraActor;								   // 0x27d8(0x08)
	char bIsOrthographic : 1;											   // 0x27e0(0x01)
	char bDefaultConstrainAspectRatio : 1;								   // 0x27e0(0x01)
	char pad_27E0_2 : 4;												   // 0x27e0(0x01)
	char bClientSimulatingViewTarget : 1;								   // 0x27e0(0x01)
	char bUseClientSideCameraUpdates : 1;								   // 0x27e0(0x01)
	char pad_27E1_0 : 2;												   // 0x27e1(0x01)
	char bGameCameraCutThisFrame : 1;									   // 0x27e1(0x01)
	char pad_27E1_3 : 5;												   // 0x27e1(0x01)
	char pad_27E2[0x2];													   // 0x27e2(0x02)
	float ViewPitchMin;													   // 0x27e4(0x04)
	float ViewPitchMax;													   // 0x27e8(0x04)
	float ViewYawMin;													   // 0x27ec(0x04)
	float ViewYawMax;													   // 0x27f0(0x04)
	float ViewRollMin;													   // 0x27f4(0x04)
	float ViewRollMax;													   // 0x27f8(0x04)
	char pad_27FC[0x4];													   // 0x27fc(0x04)
	float ServerUpdateCameraTimeout;									   // 0x2800(0x04)
	char pad_2804[0xc];													   // 0x2804(0x0c)

	void SwapPendingViewTargetWhenUsingClientSideCameraUpdates();																																																	   // Function Engine.PlayerCameraManager.SwapPendingViewTargetWhenUsingClientSideCameraUpdates // (Final|Native|Protected) // @ game+0x37b6620
	void StopCameraShake(struct UCameraShakeBase* ShakeInstance, bool bImmediately);																																												   // Function Engine.PlayerCameraManager.StopCameraShake // (Native|Public|BlueprintCallable) // @ game+0x37b64d0
	void StopCameraFade();																																																											   // Function Engine.PlayerCameraManager.StopCameraFade // (Native|Public|BlueprintCallable) // @ game+0x37b64b0
	void StopCameraAnimInst(struct UCameraAnimInst* AnimInst, bool bImmediate);																																														   // Function Engine.PlayerCameraManager.StopCameraAnimInst // (Native|Public|BlueprintCallable) // @ game+0x37b63e0
	void StopAllInstancesOfCameraShakeFromSource(struct UCameraShakeBase* Shake, struct UCameraShakeSourceComponent* SourceComponent, bool bImmediately);																											   // Function Engine.PlayerCameraManager.StopAllInstancesOfCameraShakeFromSource // (Native|Public|BlueprintCallable) // @ game+0x37b62d0
	void StopAllInstancesOfCameraShake(struct UCameraShakeBase* Shake, bool bImmediately);																																											   // Function Engine.PlayerCameraManager.StopAllInstancesOfCameraShake // (Native|Public|BlueprintCallable) // @ game+0x37b6200
	void StopAllInstancesOfCameraAnim(struct UCameraAnim* Anim, bool bImmediate);																																													   // Function Engine.PlayerCameraManager.StopAllInstancesOfCameraAnim // (Native|Public|BlueprintCallable) // @ game+0x37b6130
	void StopAllCameraShakesFromSource(struct UCameraShakeSourceComponent* SourceComponent, bool bImmediately);																																						   // Function Engine.PlayerCameraManager.StopAllCameraShakesFromSource // (Native|Public|BlueprintCallable) // @ game+0x37b6060
	void StopAllCameraShakes(bool bImmediately);																																																					   // Function Engine.PlayerCameraManager.StopAllCameraShakes // (Native|Public|BlueprintCallable) // @ game+0x37b5fd0
	void StopAllCameraAnims(bool bImmediate);																																																						   // Function Engine.PlayerCameraManager.StopAllCameraAnims // (Native|Public|BlueprintCallable) // @ game+0x37b5f40
	struct UCameraShakeBase* StartCameraShakeFromSource(struct UCameraShakeBase* ShakeClass, struct UCameraShakeSourceComponent* SourceComponent, float Scale, enum class ECameraShakePlaySpace PlaySpace, struct FRotator UserPlaySpaceRot);						   // Function Engine.PlayerCameraManager.StartCameraShakeFromSource // (Native|Public|HasDefaults|BlueprintCallable) // @ game+0x37b5cd0
	struct UCameraShakeBase* StartCameraShake(struct UCameraShakeBase* ShakeClass, float Scale, enum class ECameraShakePlaySpace PlaySpace, struct FRotator UserPlaySpaceRot);																						   // Function Engine.PlayerCameraManager.StartCameraShake // (Native|Public|HasDefaults|BlueprintCallable) // @ game+0x37b5b50
	void StartCameraFade(float FromAlpha, float ToAlpha, float Duration, struct FLinearColor Color, bool bShouldFadeAudio, bool bHoldWhenFinished);																													   // Function Engine.PlayerCameraManager.StartCameraFade // (Native|Public|HasDefaults|BlueprintCallable) // @ game+0x37b5960
	void SetManualCameraFade(float InFadeAmount, struct FLinearColor Color, bool bInFadeAudio);																																										   // Function Engine.PlayerCameraManager.SetManualCameraFade // (Native|Public|HasDefaults|BlueprintCallable) // @ game+0x37b53c0
	void SetGameCameraCutThisFrame();																																																								   // Function Engine.PlayerCameraManager.SetGameCameraCutThisFrame // (Final|Native|Public|BlueprintCallable) // @ game+0x37b5290
	bool RemoveCameraModifier(struct UCameraModifier* ModifierToRemove);																																															   // Function Engine.PlayerCameraManager.RemoveCameraModifier // (Native|Public|BlueprintCallable) // @ game+0x37b3c80
	void RemoveCameraLensEffect(struct AEmitterCameraLensEffectBase* Emitter);																																														   // Function Engine.PlayerCameraManager.RemoveCameraLensEffect // (Native|Public|BlueprintCallable) // @ game+0x1669390
	struct UCameraAnimInst* PlayCameraAnim(struct UCameraAnim* Anim, float Rate, float Scale, float BlendInTime, float BlendOutTime, bool bLoop, bool bRandomStartTime, float Duration, enum class ECameraShakePlaySpace PlaySpace, struct FRotator UserPlaySpaceRot); // Function Engine.PlayerCameraManager.PlayCameraAnim // (Native|Public|HasDefaults|BlueprintCallable) // @ game+0x37b3420
	void PhotographyCameraModify(struct FVector NewCameraLocation, struct FVector PreviousCameraLocation, struct FVector OriginalCameraLocation, struct FVector& ResultCameraLocation);																				   // Function Engine.PlayerCameraManager.PhotographyCameraModify // (BlueprintCosmetic|Native|Event|Public|HasOutParms|HasDefaults|BlueprintEvent) // @ game+0x37b3290
	void OnPhotographySessionStart();																																																								   // Function Engine.PlayerCameraManager.OnPhotographySessionStart // (BlueprintCosmetic|Native|Event|Public|BlueprintEvent) // @ game+0x165c3b0
	void OnPhotographySessionEnd();																																																									   // Function Engine.PlayerCameraManager.OnPhotographySessionEnd // (BlueprintCosmetic|Native|Event|Public|BlueprintEvent) // @ game+0x16f3390
	void OnPhotographyMultiPartCaptureStart();																																																						   // Function Engine.PlayerCameraManager.OnPhotographyMultiPartCaptureStart // (BlueprintCosmetic|Native|Event|Public|BlueprintEvent) // @ game+0x16a2a70
	void OnPhotographyMultiPartCaptureEnd();																																																						   // Function Engine.PlayerCameraManager.OnPhotographyMultiPartCaptureEnd // (BlueprintCosmetic|Native|Event|Public|BlueprintEvent) // @ game+0x164ce00
	struct APlayerController* GetOwningPlayerController();																																																			   // Function Engine.PlayerCameraManager.GetOwningPlayerController // (Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x37b2d60
	float GetFOVAngle();																																																											   // Function Engine.PlayerCameraManager.GetFOVAngle // (Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x36c9d80
	struct FRotator GetCameraRotation();																																																							   // Function Engine.PlayerCameraManager.GetCameraRotation // (Native|Public|HasDefaults|BlueprintCallable|BlueprintPure|Const) // @ game+0x37b19a0
	struct FVector GetCameraLocation();																																																								   // Function Engine.PlayerCameraManager.GetCameraLocation // (Native|Public|HasDefaults|BlueprintCallable|BlueprintPure|Const) // @ game+0x37b1960
	struct UCameraModifier* FindCameraModifierByClass(struct UCameraModifier* ModifierClass);																																										   // Function Engine.PlayerCameraManager.FindCameraModifierByClass // (Native|Public|BlueprintCallable) // @ game+0x37b18c0
	void ClearCameraLensEffects();																																																									   // Function Engine.PlayerCameraManager.ClearCameraLensEffects // (Native|Public|BlueprintCallable) // @ game+0x16a3ab0
	bool BlueprintUpdateCamera(struct AActor* CameraTarget, struct FVector& NewCameraLocation, struct FRotator& NewCameraRotation, float& NewCameraFOV);																											   // Function Engine.PlayerCameraManager.BlueprintUpdateCamera // (BlueprintCosmetic|Event|Public|HasOutParms|HasDefaults|BlueprintEvent) // @ game+0x1a5c6b0
	struct UCameraModifier* AddNewCameraModifier(struct UCameraModifier* ModifierClass);																																											   // Function Engine.PlayerCameraManager.AddNewCameraModifier // (Native|Public|BlueprintCallable) // @ game+0x37ae5e0
	struct AEmitterCameraLensEffectBase* AddCameraLensEffect(struct AEmitterCameraLensEffectBase* LensEffectEmitterClass);																																			   // Function Engine.PlayerCameraManager.AddCameraLensEffect // (Native|Public|BlueprintCallable) // @ game+0x37ae540
	void SetCameraCachePOV(const FMinimalViewInfo& InPOV);
};
