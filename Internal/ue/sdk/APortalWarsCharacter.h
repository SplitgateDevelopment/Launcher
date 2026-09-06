#pragma once

#include "Fwd.h"
#include "Values.h"
#include "ACharacter.h"

// Class PortalWars.PortalWarsCharacter
// Size: 0xde0 (Inherited: 0x4c0)
struct APortalWarsCharacter : ACharacter
{
	char pad_4C0[0x18];											  // 0x4c0(0x18)
	struct APortalWarsGameState* GameStateRef;					  // 0x4d8(0x08)
	char pad_4E0[0xc];											  // 0x4e0(0x0c)
	float Health;												  // 0x4ec(0x04)
	float MaxHealth;											  // 0x4f0(0x04)
	char pad_4F4[0x8];											  // 0x4f4(0x08)
	float healthRechargeDelay;									  // 0x4fc(0x04)
	struct UAkAudioEvent* HealthRechargeStartEvent;				  // 0x500(0x08)
	struct UAkAudioEvent* HealthRechargeStopEvent;				  // 0x508(0x08)
	struct UAkAudioEvent* LowHealthStartEvent;					  // 0x510(0x08)
	struct UAkAudioEvent* LowHealthStopEvent;					  // 0x518(0x08)
	struct UAkAudioEvent* KillHealthLoopEvents;					  // 0x520(0x08)
	char pad_528[0x10];											  // 0x528(0x10)
	float RagdollLifetime;										  // 0x538(0x04)
	char pad_53C[0xf4];											  // 0x53c(0xf4)
	char LastCausedHitInfo[0xf0];								  // 0x630(0xf0)
	char pad_720[0x10];											  // 0x720(0x10)
	struct UAnimMontage* upperBodyFlinchMontage;				  // 0x730(0x08)
	struct UAnimMontage* lowerBodyFlinchMontage;				  // 0x738(0x08)
	struct UAnimMontage* headFlinchMontage;						  // 0x740(0x08)
	char pad_748[0x8];											  // 0x748(0x08)
	struct UDamageType* SuicideDamageType;						  // 0x750(0x08)
	struct UDamageType* portalDamageType;						  // 0x758(0x08)
	struct UAkAudioEvent* PlayerTakeDamageEvent;				  // 0x760(0x08)
	struct UAkAudioEvent* RagdollCollisionEvent;				  // 0x768(0x08)
	float RagdollImpactSFXThreshold;							  // 0x770(0x04)
	float PostDeathPortalLifetime;								  // 0x774(0x04)
	char pad_778[0x10];											  // 0x778(0x10)
	struct TArray<struct APortalWarsAIController*> TargetingBots; // 0x788(0x10)
	struct UCameraComponent* ThirdPersonCamera;					  // 0x798(0x08)
	struct USpringArmComponent* ThirdPersonCameraArm;			  // 0x7a0(0x08)
	char pad_7A8[0x8];											  // 0x7a8(0x08)
	struct UCameraComponent* SpectatorFirstPersonCamera;		  // 0x7b0(0x08)
	struct USpringArmComponent* SpectatorFirstPersonCameraArm;	  // 0x7b8(0x08)
	char pad_7C0[0x20];											  // 0x7c0(0x20)
	char DefaultInventoryClasses[0x10];							  // 0x7e0(0x10)
	struct TArray<struct AGun*> Inventory;						  // 0x7f0(0x10)
	struct AGun* CurrentWeapon;									  // 0x800(0x08)
	char pad_808[0x8];											  // 0x808(0x08)
	struct AGun* Fists;											  // 0x810(0x08)
	struct AGun* FistsClass;									  // 0x818(0x08)
	struct AGun* CurrentWeaponClass;							  // 0x820(0x08)
	char pad_828[0x38];											  // 0x828(0x38)
	struct FName WeaponAttachPoint;								  // 0x860(0x08)
	struct FName GrendadeAttachPoint;							  // 0x868(0x08)
	struct APortalLauncher* portalLauncherClass;				  // 0x870(0x08)
	struct APortalLauncher* PortalLauncher;						  // 0x878(0x08)
	struct FName PortalLauncherAttachPoint;						  // 0x880(0x08)
	char pad_888[0x8];											  // 0x888(0x08)
	struct USceneComponent* FirstPersonArmsRoot;				  // 0x890(0x08)
	char pad_898[0x10];											  // 0x898(0x10)
	struct USkeletalMeshComponent* Mesh1P;						  // 0x8a8(0x08)
	struct USkeletalMeshComponent* Jetpack;						  // 0x8b0(0x08)
	char JetpackFlameMeshes[0x50];								  // 0x8b8(0x50)
	char pad_908[0x20];											  // 0x908(0x20)
	int32_t FriendlyStencilValue;								  // 0x928(0x04)
	int32_t EnemyStencilValue;									  // 0x92c(0x04)
	int32_t AlphaTeamStencilValue;								  // 0x930(0x04)
	int32_t BravoTeamStencilValue;								  // 0x934(0x04)
	struct FLinearColor BlueOutlineColor;						  // 0x938(0x10)
	struct FLinearColor RedOutlineColor;						  // 0x948(0x10)
	float ColorIntensity3P;										  // 0x958(0x04)
	float ColorIntensity1P;										  // 0x95c(0x04)
	char pad_960[0x50];											  // 0x960(0x50)
	struct ACharacterSkin* DefaultCharacterSkinClass;			  // 0x9b0(0x08)
	struct ACharacterSkin* CharacterSkin;						  // 0x9b8(0x08)
	struct ACharacterSkin* CharacterSkinClass;					  // 0x9c0(0x08)
	struct AJetpackSkin* DefaultJetpackSkinClass;				  // 0x9c8(0x08)
	struct AJetpackSkin* JetpackSkin;							  // 0x9d0(0x08)
	struct AJetpackSkin* JetpackSkinClass;						  // 0x9d8(0x08)
	char pad_9E0[0x20];											  // 0x9e0(0x20)
	struct UAnimSequence* EmoteSequenceCurrentlyPlayingInMenu;	  // 0xa00(0x08)
	float SprayRange;											  // 0xa08(0x04)
	float SprayLifetime;										  // 0xa0c(0x04)
	float TimeBetweenSprays;									  // 0xa10(0x04)
	char pad_A14[0x4];											  // 0xa14(0x04)
	struct TArray<struct FSavedPosition> SavedPositions;		  // 0xa18(0x10)
	char pad_A28[0x8];											  // 0xa28(0x08)
	char bIsSprinting : 1;										  // 0xa30(0x01)
	char pad_A30_1 : 7;											  // 0xa30(0x01)
	char pad_A31[0x7];											  // 0xa31(0x07)
	struct UMatineeCameraShake* SprintCamShake;					  // 0xa38(0x08)
	char pad_A40[0x4];											  // 0xa40(0x04)
	float SprintingSpeedModifier;								  // 0xa44(0x04)
	char pad_A48[0x10];											  // 0xa48(0x10)
	struct APortalWarsTeabagZone* TeabagZone;					  // 0xa58(0x08)
	char pad_A60[0x8];											  // 0xa60(0x08)
	float CrouchCameraSpeed;									  // 0xa68(0x04)
	char pad_A6C[0x4];											  // 0xa6c(0x04)
	struct UAkAudioEvent* CrouchEvent;							  // 0xa70(0x08)
	struct UAkAudioEvent* UncrouchEvent;						  // 0xa78(0x08)
	float BaseTurnRate;											  // 0xa80(0x04)
	float BaseLookUpRate;										  // 0xa84(0x04)
	char pad_A88[0x98];											  // 0xa88(0x98)
	float ControllerMaxAccelMultiplier;							  // 0xb20(0x04)
	char pad_B24[0x74];											  // 0xb24(0x74)
	float rotateToUprightSpeed;									  // 0xb98(0x04)
	char DoRep_collisionProfile;								  // 0xb9c(0x01)
	char pad_B9D[0x13];											  // 0xb9d(0x13)
	struct UAkAudioEvent* TeleportEvent;						  // 0xbb0(0x08)
	struct UAkAudioEvent* LocalPlayerTeleportEvent;				  // 0xbb8(0x08)
	char pad_BC0[0x10];											  // 0xbc0(0x10)
	struct AGrenadeLauncher* GrenadeLauncher;					  // 0xbd0(0x08)
	struct AGrenadeLauncher* GrenadeLauncherClass;				  // 0xbd8(0x08)
	struct UAkAudioEvent* ThrowGrenadeEvent;					  // 0xbe0(0x08)
	char IgnoreServerCorrections[0x02];							  // 0xbe8(0x02)
	char pad_BEA[0x6];											  // 0xbea(0x06)
	float MeleeApplyDmgRange;									  // 0xbf0(0x04)
	float MeleeApplyDmgRangeXY;									  // 0xbf4(0x04)
	float MeleeApplyDmgAngle;									  // 0xbf8(0x04)
	float MeleeRange;											  // 0xbfc(0x04)
	float TimeBetweenMelee;										  // 0xc00(0x04)
	float MeleeConeHalfAngle;									  // 0xc04(0x04)
	char pad_C08[0x34];											  // 0xc08(0x34)
	float MeleeMaxRotationAngle;								  // 0xc3c(0x04)
	char pad_C40[0x10];											  // 0xc40(0x10)
	struct UDamageType* MeleeDamageType;						  // 0xc50(0x08)
	char pad_C58[0x10];											  // 0xc58(0x10)
	struct UAkAudioEvent* InstantMeleeSpeedReachedEvent;		  // 0xc68(0x08)
	struct UAkAudioEvent* InstantMeleeSpeedStopEvent;			  // 0xc70(0x08)
	char pad_C78[0x1];											  // 0xc78(0x01)
	char bIsThrusting : 1;										  // 0xc79(0x01)
	char pad_C79_1 : 7;											  // 0xc79(0x01)
	char pad_C7A[0x2];											  // 0xc7a(0x02)
	float thrustAmountPerTick;									  // 0xc7c(0x04)
	float thrusterVelocityThreshhold;							  // 0xc80(0x04)
	float thrusterRechargeDelay;								  // 0xc84(0x04)
	float thrusterTotalTime;									  // 0xc88(0x04)
	float thrusterCurrentTime;									  // 0xc8c(0x04)
	char pad_C90[0x8];											  // 0xc90(0x08)
	struct UAkAudioEvent* JetPackStartEvent;					  // 0xc98(0x08)
	struct UAkAudioEvent* JetPackEndEvent;						  // 0xca0(0x08)
	char pad_CA8[0x10];											  // 0xca8(0x10)
	char bIsClambering : 1;										  // 0xcb8(0x01)
	char pad_CB8_1 : 7;											  // 0xcb8(0x01)
	char pad_CB9[0x3];											  // 0xcb9(0x03)
	float ClamberCameraShakeDelay;								  // 0xcbc(0x04)
	struct UCameraShakeBase* ClamberCameraShake;				  // 0xcc0(0x08)
	char bIsZooming : 1;										  // 0xcc8(0x01)
	char pad_CC8_1 : 7;											  // 0xcc8(0x01)
	char pad_CC9[0x3];											  // 0xcc9(0x03)
	float maxTimeOutOfBounds;									  // 0xccc(0x04)
	float curTimeOutOfBounds;									  // 0xcd0(0x04)
	char pad_CD4[0x4];											  // 0xcd4(0x04)
	struct UAkAudioEvent* OutOfBoundsStartEvent;				  // 0xcd8(0x08)
	struct UAkAudioEvent* OutofBoundsEndEvent;					  // 0xce0(0x08)
	char pad_CE8[0x8];											  // 0xce8(0x08)
	struct UPortalWarsAnimInstanceV2* Animation;				  // 0xcf0(0x08)
	char pad_CF8[0x20];											  // 0xcf8(0x20)
	struct UAkAudioEvent* DeathEvent;							  // 0xd18(0x08)
	struct UAkAudioEvent* RespawnEvent;							  // 0xd20(0x08)
	struct UAkAudioEvent* SprayEvent;							  // 0xd28(0x08)
	struct UAkAudioEvent* Footstep3pEvent;						  // 0xd30(0x08)
	struct UAkAudioEvent* Footstep1pEvent;						  // 0xd38(0x08)
	struct UAkAudioEvent* Land3pEvent;							  // 0xd40(0x08)
	struct UAkAudioEvent* Land1pEvent;							  // 0xd48(0x08)
	struct UAkAudioEvent* Jump3pEvent;							  // 0xd50(0x08)
	struct UAkAudioEvent* Jump1pEvent;							  // 0xd58(0x08)
	struct UPortalWarsAkComponent* AkFirstPerson;				  // 0xd60(0x08)
	struct UPortalWarsAkComponent* AkThirdPerson;				  // 0xd68(0x08)
	struct UPortalWarsAkComponent* AkFoot;						  // 0xd70(0x08)
	struct UPortalWarsAkComponent* AkJetPack;					  // 0xd78(0x08)
	struct UPortalWarsAkComponent* AkGunLocation;				  // 0xd80(0x08)
	struct UPortalWarsAkComponent* AkMoveable;					  // 0xd88(0x08)
	struct UAkAudioEvent* RespawnCountdownEvent;				  // 0xd90(0x08)
	struct UAkAudioEvent* RespawnCountdownStopEvent;			  // 0xd98(0x08)
	float FootstepLoudnessForBots;								  // 0xda0(0x04)
	float ThrusterLoudnessForBots;								  // 0xda4(0x04)
	char pad_DA8[0x8];											  // 0xda8(0x08)
	struct UPortalWarsIndicatorWidget* NameIndicatorWidget;		  // 0xdb0(0x08)
	struct UPortalWarsRadarMarkerWidget* RadarMarkerWidget;		  // 0xdb8(0x08)
	struct APortalWarsPlayerState* LastPlayerState;				  // 0xdc0(0x08)
	char pad_DC8[0x4];											  // 0xdc8(0x04)
	uint16_t RemotePitch;										  // 0xdcc(0x02)
	uint16_t RemoteYaw;											  // 0xdce(0x02)
	uint16_t RemoteRoll;										  // 0xdd0(0x02)
	char pad_DD2[0xe];											  // 0xdd2(0x0e)

	void UpdateSkins();																																								  // Function PortalWars.PortalWarsCharacter.UpdateSkins // (Final|Native|Protected) // @ game+0x166a4a0
	void updateCollisionProfiles();																																					  // Function PortalWars.PortalWarsCharacter.updateCollisionProfiles // (Final|Native|Protected) // @ game+0x166a7b0
	void StopEmoteInMenu();																																							  // Function PortalWars.PortalWarsCharacter.StopEmoteInMenu // (Final|Native|Protected) // @ game+0x166a480
	void StopEmote();																																								  // Function PortalWars.PortalWarsCharacter.StopEmote // (Final|Native|Protected) // @ game+0x166a460
	void SpawnSpray_Multicast(struct FVector_NetQuantize Location, struct FVector_NetQuantizeNormal Rotation);																		  // Function PortalWars.PortalWarsCharacter.SpawnSpray_Multicast // (Net|Native|Event|NetMulticast|Protected) // @ game+0x166a370
	void ServerTeleport(struct FTransform_NetQuantize CharacterTransform_world, struct FVector_NetQuantize NewVel, struct FQuat NewControlRotation, struct APortal* PortalUsed);	  // Function PortalWars.PortalWarsCharacter.ServerTeleport // (Net|NetReliableNative|Event|Public|NetServer|HasDefaults|NetValidate) // @ game+0x166a180
	void ServerStartMelee(struct APortalWarsCharacter* InEnemyToMelee, struct FVector_NetQuantize InVelocity);																		  // Function PortalWars.PortalWarsCharacter.ServerStartMelee // (Net|NetReliableNative|Event|Protected|NetServer|NetValidate) // @ game+0x166a080
	void ServerSpawnSpray(struct FVector_NetQuantize Location, struct FVector_NetQuantizeNormal Rotation);																			  // Function PortalWars.PortalWarsCharacter.ServerSpawnSpray // (Net|Native|Event|Protected|NetServer|NetValidate) // @ game+0x1669f40
	void ServerSetZooming(bool bNewZooming);																																		  // Function PortalWars.PortalWarsCharacter.ServerSetZooming // (Net|NetReliableNative|Event|Protected|NetServer|NetValidate) // @ game+0x1669e80
	void ServerSetOverlappingPortalCollisionProfiles();																																  // Function PortalWars.PortalWarsCharacter.ServerSetOverlappingPortalCollisionProfiles // (Net|NetReliableNative|Event|Public|NetServer|NetValidate) // @ game+0x1669e30
	void ServerSetDefaultCollisionProfiles();																																		  // Function PortalWars.PortalWarsCharacter.ServerSetDefaultCollisionProfiles // (Net|NetReliableNative|Event|Public|NetServer|NetValidate) // @ game+0x1669de0
	void ServerRequestSuicide();																																					  // Function PortalWars.PortalWarsCharacter.ServerRequestSuicide // (Net|Native|Event|Public|NetServer|NetValidate) // @ game+0x1669d90
	void ServerPlayEmote();																																							  // Function PortalWars.PortalWarsCharacter.ServerPlayEmote // (Net|Native|Event|Protected|NetServer|NetValidate) // @ game+0x1669d40
	void ServerInterruptEmote();																																					  // Function PortalWars.PortalWarsCharacter.ServerInterruptEmote // (Net|Native|Event|Protected|NetServer|NetValidate) // @ game+0x1669cf0
	void ServerHandleTeabagging(struct TArray<struct APortalWarsCharacter*> deadBodies);																							  // Function PortalWars.PortalWarsCharacter.ServerHandleTeabagging // (Net|NetReliableNative|Event|Public|NetServer|NetValidate) // @ game+0x1669c30
	void ServerFinishMelee(struct FVector_NetQuantize100 EndingLocation);																											  // Function PortalWars.PortalWarsCharacter.ServerFinishMelee // (Net|NetReliableNative|Event|Protected|NetServer|NetValidate) // @ game+0x1669b60
	void ServerEquipWeapon(struct AGun* NewWeapon);																																	  // Function PortalWars.PortalWarsCharacter.ServerEquipWeapon // (Net|NetReliableNative|Event|Protected|NetServer|NetValidate) // @ game+0x1669aa0
	void ServerAttemptManualPickup(struct APortalWarsGunPickup* GunPickup, struct FVector_NetQuantize CurrentWeaponLocation, struct FVector_NetQuantizeNormal CurrentWeaponRotation); // Function PortalWars.PortalWarsCharacter.ServerAttemptManualPickup // (Net|NetReliableNative|Event|Public|NetServer|NetValidate) // @ game+0x1669920
	void ServerApplyMeleeDamage(struct APortalWarsCharacter* EnemyToDamage);																										  // Function PortalWars.PortalWarsCharacter.ServerApplyMeleeDamage // (Net|NetReliableNative|Event|Protected|NetServer|NetValidate) // @ game+0x1669860
	void RequestSuicide();																																							  // Function PortalWars.PortalWarsCharacter.RequestSuicide // (Final|Native|Public|BlueprintCallable) // @ game+0x1669840
	void PlayEmoteInMenu_Internal();																																				  // Function PortalWars.PortalWarsCharacter.PlayEmoteInMenu_Internal // (Final|Native|Protected) // @ game+0x16697a0
	void PlayEmote_Multicast();																																						  // Function PortalWars.PortalWarsCharacter.PlayEmote_Multicast // (Net|Native|Event|NetMulticast|Protected) // @ game+0x16697c0
	void OnSpray();																																									  // Function PortalWars.PortalWarsCharacter.OnSpray // (Native|Public) // @ game+0x16696f0
	void OnReplayCameraModeChanged(enum class EReplayCameraMode NewReplayCameraMode);																								  // Function PortalWars.PortalWarsCharacter.OnReplayCameraModeChanged // (Final|Native|Public) // @ game+0x1669670
	void OnRep_PortalGun();																																							  // Function PortalWars.PortalWarsCharacter.OnRep_PortalGun // (Final|Native|Protected) // @ game+0x1669650
	void OnRep_LastCausedHitInfo();																																					  // Function PortalWars.PortalWarsCharacter.OnRep_LastCausedHitInfo // (Final|Native|Public) // @ game+0x1669630
	void OnRep_IsZooming();																																							  // Function PortalWars.PortalWarsCharacter.OnRep_IsZooming // (Final|Native|Protected) // @ game+0x1669610
	void OnRep_IsThrusting();																																						  // Function PortalWars.PortalWarsCharacter.OnRep_IsThrusting // (Native|Public) // @ game+0x16695f0
	void OnRep_IsSprinting();																																						  // Function PortalWars.PortalWarsCharacter.OnRep_IsSprinting // (Final|Native|Protected) // @ game+0x16695d0
	void OnRep_IsClambering();																																						  // Function PortalWars.PortalWarsCharacter.OnRep_IsClambering // (Final|Native|Protected) // @ game+0x16695b0
	void OnRep_Inventory();																																							  // Function PortalWars.PortalWarsCharacter.OnRep_Inventory // (Final|Native|Protected) // @ game+0x1669590
	void OnRep_IgnoreServerCorrections();																																			  // Function PortalWars.PortalWarsCharacter.OnRep_IgnoreServerCorrections // (Final|Native|Protected) // @ game+0x1669570
	void OnRep_Health();																																							  // Function PortalWars.PortalWarsCharacter.OnRep_Health // (Final|Native|Public) // @ game+0x1669550
	void OnRep_GrenadeLauncher();																																					  // Function PortalWars.PortalWarsCharacter.OnRep_GrenadeLauncher // (Final|Native|Protected) // @ game+0x1669530
	void OnRep_CurTimeOutOfBounds();																																				  // Function PortalWars.PortalWarsCharacter.OnRep_CurTimeOutOfBounds // (Final|Native|Protected) // @ game+0x1669480
	void OnRep_CurrentWeapon(struct AGun* LastWeapon);																																  // Function PortalWars.PortalWarsCharacter.OnRep_CurrentWeapon // (Final|Native|Protected) // @ game+0x16694a0
	void OnKillcamStarted();																																						  // Function PortalWars.PortalWarsCharacter.OnKillcamStarted // (Final|Native|Public) // @ game+0x1669440
	void OnKillcamEnded();																																							  // Function PortalWars.PortalWarsCharacter.OnKillcamEnded // (Final|Native|Public) // @ game+0x1669420
	void OnGlobalVisionChanged();																																					  // Function PortalWars.PortalWarsCharacter.OnGlobalVisionChanged // (Final|Native|Public) // @ game+0x1669250
	void OnGameplaySettingsChanged();																																				  // Function PortalWars.PortalWarsCharacter.OnGameplaySettingsChanged // (Final|Native|Protected) // @ game+0x1669230
	void OnGameConfigUpdated();																																						  // Function PortalWars.PortalWarsCharacter.OnGameConfigUpdated // (Native|Protected) // @ game+0x1669210
	void OnDeath();																																									  // Function PortalWars.PortalWarsCharacter.OnDeath // (Final|Native|Public) // @ game+0x16691f0
	void MoveAndPostToMoveableAk(struct UAkAudioEvent* AkEvent, struct FVector NewLocation);																						  // Function PortalWars.PortalWarsCharacter.MoveAndPostToMoveableAk // (Final|Native|Public|HasDefaults|BlueprintCallable) // @ game+0x1668d80
	bool IsZooming();																																								  // Function PortalWars.PortalWarsCharacter.IsZooming // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x1668d50
	bool IsThirdPerson();																																							  // Function PortalWars.PortalWarsCharacter.IsThirdPerson // (Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x1668d20
	bool IsSprinting();																																								  // Function PortalWars.PortalWarsCharacter.IsSprinting // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x1668cf0
	bool IsLocallyViewed();																																							  // Function PortalWars.PortalWarsCharacter.IsLocallyViewed // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x1668cc0
	bool IsFirstPerson();																																							  // Function PortalWars.PortalWarsCharacter.IsFirstPerson // (Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x1668c90
	bool IsFiring();																																								  // Function PortalWars.PortalWarsCharacter.IsFiring // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x1668c60
	bool IsEnemyFor(struct APortalWarsCharacter* Character);																														  // Function PortalWars.PortalWarsCharacter.IsEnemyFor // (Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x1668bc0
	bool IsDead();																																									  // Function PortalWars.PortalWarsCharacter.IsDead // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x1668b90
	bool IsAlive();																																									  // Function PortalWars.PortalWarsCharacter.IsAlive // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x1668b60
	void InterruptEmote_Multicast();																																				  // Function PortalWars.PortalWarsCharacter.InterruptEmote_Multicast // (Net|Native|Event|NetMulticast|Protected) // @ game+0x1668b40
	struct AGun* GetWeapon();																																						  // Function PortalWars.PortalWarsCharacter.GetWeapon // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x1668aa0
	char GetTeamNum();																																								  // Function PortalWars.PortalWarsCharacter.GetTeamNum // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x1668900
	float GetSprintingSpeedModifier();																																				  // Function PortalWars.PortalWarsCharacter.GetSprintingSpeedModifier // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x16688e0
	struct AGun* GetSecondaryWeapon();																																				  // Function PortalWars.PortalWarsCharacter.GetSecondaryWeapon // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x16688b0
	struct USkeletalMeshComponent* GetMesh1P();																																		  // Function PortalWars.PortalWarsCharacter.GetMesh1P // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x1668890
	char GetEnemyTeamNum();																																							  // Function PortalWars.PortalWarsCharacter.GetEnemyTeamNum // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x1668860
	struct UPortalWarsAkComponent* GetAkComponent(struct FName AkName);																												  // Function PortalWars.PortalWarsCharacter.GetAkComponent // (Final|Native|Public|BlueprintCallable) // @ game+0x1668620
	struct FRotator GetAimOffsets();																																				  // Function PortalWars.PortalWarsCharacter.GetAimOffsets // (Final|Native|Public|HasDefaults|BlueprintCallable|BlueprintPure) // @ game+0x16685e0
	void DestroyPortals();																																							  // Function PortalWars.PortalWarsCharacter.DestroyPortals // (Final|Native|Public) // @ game+0x16685c0
	void DelayedServerFinishMelee();																																				  // Function PortalWars.PortalWarsCharacter.DelayedServerFinishMelee // (Final|Native|Protected) // @ game+0x16685a0
	void ClientNotifyAlive(struct AController* C);																																	  // Function PortalWars.PortalWarsCharacter.ClientNotifyAlive // (Net|NetReliableNative|Event|Public|NetClient) // @ game+0x1668510
	void ClientFailedPickupAttempt();																																				  // Function PortalWars.PortalWarsCharacter.ClientFailedPickupAttempt // (Net|NetReliableNative|Event|Public|NetClient) // @ game+0x16684f0
	void ClientEquipWeapon(struct AGun* NewWeapon);																																	  // Function PortalWars.PortalWarsCharacter.ClientEquipWeapon // (Net|NetReliableNative|Event|Protected|NetClient) // @ game+0x1668460
	void ClientDropSpecialItem(struct ASpecialItem* SpecialItem);																													  // Function PortalWars.PortalWarsCharacter.ClientDropSpecialItem // (Net|NetReliableNative|Event|Public|NetClient) // @ game+0x16683d0
	void ClientDebugPortalRip();																																					  // Function PortalWars.PortalWarsCharacter.ClientDebugPortalRip // (Net|NetReliableNative|Event|Public|NetClient) // @ game+0x16683b0
	void CheatSuicide();																																							  // Function PortalWars.PortalWarsCharacter.CheatSuicide // (Final|Exec|Native|Protected) // @ game+0xf085f0
	void CheatShootAll();																																							  // Function PortalWars.PortalWarsCharacter.CheatShootAll // (Final|Exec|Native|Protected) // @ game+0xf085f0
	void CheatProjectiles();																																						  // Function PortalWars.PortalWarsCharacter.CheatProjectiles // (Final|Exec|Native|Protected) // @ game+0xf085f0
	void CheatPortalNoClip();																																						  // Function PortalWars.PortalWarsCharacter.CheatPortalNoClip // (Final|Exec|Native|Protected) // @ game+0xf085f0
	void CheatPickup();																																								  // Function PortalWars.PortalWarsCharacter.CheatPickup // (Final|Exec|Native|Protected) // @ game+0xf085f0
	void CheatMelee();																																								  // Function PortalWars.PortalWarsCharacter.CheatMelee // (Final|Exec|Native|Protected) // @ game+0xf085f0
	void CheatAutoPickup();																																							  // Function PortalWars.PortalWarsCharacter.CheatAutoPickup // (Final|Exec|Native|Protected) // @ game+0xf085f0
	void applyShotgunPointImpulsesToCorpseMulticast(struct TArray<struct FPWPointDamageEvent> DamageEvents);																		  // Function PortalWars.PortalWarsCharacter.applyShotgunPointImpulsesToCorpseMulticast // (Net|Native|Event|NetMulticast|Public) // @ game+0x166a6e0
	void applyRadialImpulseToCorpseMulticast(struct FPWRadialDamageEvent RadialDamageEvent);																						  // Function PortalWars.PortalWarsCharacter.applyRadialImpulseToCorpseMulticast // (Net|Native|Event|NetMulticast|Public) // @ game+0x166a600
	void applyPointImpulseToCorpseMulticast(struct FPWPointDamageEvent PointDamageEvent);																							  // Function PortalWars.PortalWarsCharacter.applyPointImpulseToCorpseMulticast // (Net|Native|Event|NetMulticast|Public) // @ game+0x166a500

	static constexpr const char* ClassName = "Class PortalWars.PortalWarsCharacter";
};
