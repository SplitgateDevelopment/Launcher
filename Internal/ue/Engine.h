#pragma once
#include <Windows.h>
#include <cstdint>
#include <string>
#include <vector>
#include <iostream>

struct FNameEntryHandle {
	uint32_t Block = 0;
	uint32_t Offset = 0;

	FNameEntryHandle(uint32_t block, uint32_t offset) : Block(block), Offset(offset) {};
	FNameEntryHandle(uint32_t id) : Block(id >> 16), Offset(id & 65535) {};
	operator uint32_t() const { return (Block << 16 | Offset); }
};

struct FNameEntry {
	uint16_t bIsWide : 1;
	uint16_t LowercaseProbeHash : 5;
	uint16_t Len : 10;
	union
	{
		char AnsiName[1024];
		wchar_t	WideName[1024];
	};

	std::string String();
};

struct FNamePool
{
	BYTE Lock[8];
	uint32_t CurrentBlock;
	uint32_t CurrentByteCursor;
	BYTE* Blocks[8192];

	FNameEntry* GetEntry(FNameEntryHandle handle) const;
};

struct FName {
	uint32_t Index;
	uint32_t Number;

	std::string GetName();
};


struct UObject {
	void** VFTable;
	uint32_t ObjectFlags;
	uint32_t InternalIndex;
	struct UClass* ClassPrivate;
	FName NamePrivate;
	UObject* OuterPrivate;

	std::string GetName();
	std::string GetFullName();
	bool IsA(void* cmp);
	void ProcessEvent(void* fn, void* parms);
};

// Class CoreUObject.Field
// Size: 0x30 (Inherited: 0x28)
struct UField : UObject {
	char UnknownData_28[0x8]; // 0x28(0x08)
};

// Class CoreUObject.Struct
// Size: 0xb0 (Inherited: 0x30)
struct UStruct : UField {
	char pad_30[0x10]; // 0x30(0x10)
	UStruct* SuperStruct; // 0x40(0x8)
	char UnknownData_48[0x68]; // 0x48(0x80)
};

// Class CoreUObject.Class
// Size: 0x230 (Inherited: 0xb0)
struct UClass : UStruct {
	char UnknownData_B0[0x180]; // 0xb0(0x180)
};

struct TUObjectArray {
	BYTE** Objects;
	BYTE* PreAllocatedObjects;
	uint32_t MaxElements;
	uint32_t NumElements;
	uint32_t MaxChunks;
	uint32_t NumChunks;

	UObject* GetObjectPtr(uint32_t id) const;
	UObject* FindObject(const char* name) const;
};

template<class T>
struct TArray
{
	friend struct FString;

public:
	inline int Num() const
	{
		return Count;
	};

	inline T& operator[](int i)
	{
		return Data[i];
	};

	inline bool IsValidIndex(int i) const
	{
		return i < Num();
	}

private:
	T* Data;
	int32_t Count;
	int32_t Max;
};

struct FString : private TArray<wchar_t>
{
	inline FString()
	{
	};

	FString(const wchar_t* other)
	{
		Max = Count = *other ? std::wcslen(other) + 1 : 0;

		if (Count)
		{
			Data = const_cast<wchar_t*>(other);
		}
	};

	inline bool IsValid() const
	{
		return Data != nullptr;
	}

	inline const wchar_t* c_str() const
	{
		return Data;
	}

	std::string ToString() const
	{
		auto length = std::wcslen(Data);

		std::string str(length, '\0');

		std::use_facet<std::ctype<wchar_t>>(std::locale()).narrow(Data, Data + length, '?', &str[0]);

		return str;
	}
};

struct FLinearColor
{
	float R;
	float G;
	float B;
	float A;
};

struct FMatrix {
	float M[4][4];
};

struct FVector2D
{
	float X;
	float Y;
};

struct FVector {
	float X;
	float Y;
	float Z;
};

struct FRotator {
	float Pitch;
	float Yaw;
	float Roll;
};

struct FRecoilData {
	float recoilRiseTime;
	float recoilTotalTime;
	float verticalRecoilAmount;
	float horizontalRecoilAmount;
	float recoilKick;
	float visualRecoil;
};

// Class Engine.Actor 
struct AActor : UObject {
	char pad_0000[0x30]; // 0x28 (0x30) 
	BYTE bTearOff; // 0x58 (0x01)
	char pad_0001[0xBF]; // 0x59 (0xBF) 
	class APawn* Instigator; // 0x118 (0x08) 
	char pad_0002[0x10]; // 0x120 (0x10)
	class USceneComponent* RootComponent; // 0x130 (0x08)
	bool K2_TeleportTo(struct FVector DestLocation, struct FRotator DestRotation); // Function Engine.Actor.K2_TeleportTo // (Final|Native|Public|HasDefaults|BlueprintCallable) // @ game+0x368c880
	struct FRotator K2_GetActorRotation(); // Function Engine.Actor.K2_GetActorRotation // (Final|Native|Public|HasDefaults|BlueprintCallable|BlueprintPure|Const) // @ game+0x368ba80
	struct FVector K2_GetActorLocation(); // Function Engine.Actor.K2_GetActorLocation // (Final|Native|Public|HasDefaults|BlueprintCallable|BlueprintPure|Const) // @ game+0x368ba00
	void SetActorEnableCollision(bool bNewActorEnableCollision);
	bool GetActorEnableCollision();
	bool K2_SetActorLocation(FVector NewLocation, bool bSweep, bool bTeleport);
	void SetActorHiddenInGame(bool bNewHidden);
};

// Class Engine.Controller 
struct AController : AActor {
	char pad_0000[0xF0]; // 0x138 (0xF0)
	class APlayerState* PlayerState; // 0x228(0x08)
	bool LineOfSightTo(AActor* Other);
	void SetControlRotation(FRotator NewRotation);
};

// Class Engine.PlayerController
struct APlayerController : AController {
	char pad_0000[0x68]; // 0x230 (0x68)
	class UPlayer* Player; // 0x298 (0x08)
	class APawn* AcknowledgedPawn; // 0x2a0 (0x08)
	bool ProjectWorldLocationToScreen(FVector& WorldLocation, FVector2D& ScreenLocation);
	void GetViewportSize(INT& X, INT& Y);
	void SwitchLevel(struct FString URL); // Function Engine.PlayerController.SwitchLevel // (Exec|Native|Public) // @ game+0x37b6640
	void FOV(float NewFOV); // Function Engine.PlayerController.FOV // (Exec|Native|Public) // @ game+0x37b1830
	void EnableCheats(); // Function Engine.PlayerController.EnableCheats // (Exec|Native|Public) // @ game+0x37b1810
	class APlayerCameraManager* PlayerCameraManager;
	void SetName(FString S); // Function Engine.PlayerController.SetName // (Exec|Native|Public) // @ game+0x37b5690
};

// Class Engine.Pawn 
struct APawn : AActor {
	char pad_0000[0x108]; // 0x138 (0x108)
	class APlayerState* PlayerState; // 0x240 (0x08)
	char pad_0001[0x10]; // 0x248 (0x10)
	class AController* Controller; // 0x258 (0x08)
	char pad_0002[0x20];// 0x260 (0x20)
	class USkeletalMeshComponent* Mesh; // 0x280 (0x08) 
	char pad_0003[0x264];// 0x288 (0x264)
	float Health; // 0x4ec (0x04)
	float MaxHealth;
	float healthRechargeDelay;
	char pad_0004[0x300];// 0x4F0 (0x300)
	struct UAkAudioEvent* HealthRechargeStartEvent; // 0x500(0x08)
	struct UAkAudioEvent* HealthRechargeStopEvent; // 0x508(0x08)
	struct UAkAudioEvent* LowHealthStartEvent; // 0x510(0x08)
	struct UAkAudioEvent* LowHealthStopEvent; // 0x518(0x08)
	struct UAkAudioEvent* KillHealthLoopEvents; // 0x520(0x08)
	char pad_528[0x10]; // 0x528(0x10)
	float RagdollLifetime; // 0x538(0x04)
	char pad_53C[0xf4]; // 0x53c(0xf4)
	char LastCausedHitInfo[0xf0]; // 0x630(0xf0)
	char pad_720[0x10]; // 0x720(0x10)
	struct UAnimMontage* upperBodyFlinchMontage; // 0x730(0x08)
	struct UAnimMontage* lowerBodyFlinchMontage; // 0x738(0x08)
	struct UAnimMontage* headFlinchMontage; // 0x740(0x08)
	char pad_748[0x8]; // 0x748(0x08)
	struct UDamageType* SuicideDamageType; // 0x750(0x08)
	struct UDamageType* portalDamageType; // 0x758(0x08)
	struct UAkAudioEvent* PlayerTakeDamageEvent; // 0x760(0x08)
	struct UAkAudioEvent* RagdollCollisionEvent; // 0x768(0x08)
	float RagdollImpactSFXThreshold; // 0x770(0x04)
	float PostDeathPortalLifetime; // 0x774(0x04)
	char pad_778[0x10]; // 0x778(0x10)
	struct TArray<struct APortalWarsAIController*> TargetingBots; // 0x788(0x10)
	struct UCameraComponent* ThirdPersonCamera; // 0x798(0x08)
	struct USpringArmComponent* ThirdPersonCameraArm; // 0x7a0(0x08)
	char pad_7A8[0x8]; // 0x7a8(0x08)
	struct UCameraComponent* SpectatorFirstPersonCamera; // 0x7b0(0x08)
	struct USpringArmComponent* SpectatorFirstPersonCameraArm; // 0x7b8(0x08)
	char pad_7C0[0x20]; // 0x7c0(0x20)
	char DefaultInventoryClasses[0x10]; // 0x7e0(0x10)
	struct TArray<struct AGun*> Inventory; // 0x7f0(0x10)
	struct AGun* CurrentWeapon; // 0x800(0x08)
	char pad_808[0x8]; // 0x808(0x08)
	struct AGun* Fists; // 0x810(0x08)
	struct AGun* FistsClass; // 0x818(0x08)
	struct AGun* CurrentWeaponClass; // 0x820(0x08)
	char pad_828[0x38]; // 0x828(0x38)
	struct FName WeaponAttachPoint; // 0x860(0x08)
	struct FName GrendadeAttachPoint; // 0x868(0x08)
	struct APortalLauncher* portalLauncherClass; // 0x870(0x08)
	struct APortalLauncher* PortalLauncher; // 0x878(0x08)
	struct FName PortalLauncherAttachPoint; // 0x880(0x08)
	char pad_888[0x8]; // 0x888(0x08)
	struct USceneComponent* FirstPersonArmsRoot; // 0x890(0x08)
	char pad_898[0x10]; // 0x898(0x10)
	struct USkeletalMeshComponent* Mesh1P; // 0x8a8(0x08)
	struct USkeletalMeshComponent* Jetpack; // 0x8b0(0x08)
	char JetpackFlameMeshes[0x50]; // 0x8b8(0x50)
	char pad_908[0x20]; // 0x908(0x20)
	int32_t FriendlyStencilValue; // 0x928(0x04)
	int32_t EnemyStencilValue; // 0x92c(0x04)
	int32_t AlphaTeamStencilValue; // 0x930(0x04)
	int32_t BravoTeamStencilValue; // 0x934(0x04)
	struct FLinearColor BlueOutlineColor; // 0x938(0x10)
	struct FLinearColor RedOutlineColor; // 0x948(0x10)
	float ColorIntensity3P; // 0x958(0x04)
	float ColorIntensity1P; // 0x95c(0x04)
	char pad_960[0x50]; // 0x960(0x50)
	struct ACharacterSkin* DefaultCharacterSkinClass; // 0x9b0(0x08)
	struct ACharacterSkin* CharacterSkin; // 0x9b8(0x08)
	struct ACharacterSkin* CharacterSkinClass; // 0x9c0(0x08)
	struct AJetpackSkin* DefaultJetpackSkinClass; // 0x9c8(0x08)
	struct AJetpackSkin* JetpackSkin; // 0x9d0(0x08)
	struct AJetpackSkin* JetpackSkinClass; // 0x9d8(0x08)
	char pad_9E0[0x20]; // 0x9e0(0x20)
	struct UAnimSequence* EmoteSequenceCurrentlyPlayingInMenu; // 0xa00(0x08)
	float SprayRange; // 0xa08(0x04)
	float SprayLifetime; // 0xa0c(0x04)
	float TimeBetweenSprays; // 0xa10(0x04)
	char pad_A14[0x4]; // 0xa14(0x04)
	struct TArray<struct FSavedPosition> SavedPositions; // 0xa18(0x10)
	char pad_A28[0x8]; // 0xa28(0x08)
	char bIsSprinting : 1; // 0xa30(0x01)
	char pad_A30_1 : 7; // 0xa30(0x01)
	char pad_A31[0x7]; // 0xa31(0x07)
	struct UMatineeCameraShake* SprintCamShake; // 0xa38(0x08)
	char pad_A40[0x4]; // 0xa40(0x04)
	float SprintingSpeedModifier; // 0xa44(0x04)
	char pad_A48[0x10]; // 0xa48(0x10)
	struct APortalWarsTeabagZone* TeabagZone; // 0xa58(0x08)
	char pad_A60[0x8]; // 0xa60(0x08)
	float CrouchCameraSpeed; // 0xa68(0x04)
	char pad_A6C[0x4]; // 0xa6c(0x04)
	struct UAkAudioEvent* CrouchEvent; // 0xa70(0x08)
	struct UAkAudioEvent* UncrouchEvent; // 0xa78(0x08)
	float BaseTurnRate; // 0xa80(0x04)
	float BaseLookUpRate; // 0xa84(0x04)
	char pad_A88[0x98]; // 0xa88(0x98)
	float ControllerMaxAccelMultiplier; // 0xb20(0x04)
	char pad_B24[0x74]; // 0xb24(0x74)
	float rotateToUprightSpeed; // 0xb98(0x04)
	char DoRep_collisionProfile; // 0xb9c(0x01)
	char pad_B9D[0x13]; // 0xb9d(0x13)
	struct UAkAudioEvent* TeleportEvent; // 0xbb0(0x08)
	struct UAkAudioEvent* LocalPlayerTeleportEvent; // 0xbb8(0x08)
	char pad_BC0[0x10]; // 0xbc0(0x10)
	struct AGrenadeLauncher* GrenadeLauncher; // 0xbd0(0x08)
	struct AGrenadeLauncher* GrenadeLauncherClass; // 0xbd8(0x08)
	struct UAkAudioEvent* ThrowGrenadeEvent; // 0xbe0(0x08)
	char IgnoreServerCorrections[0x02]; // 0xbe8(0x02)
	char pad_BEA[0x6]; // 0xbea(0x06)
	float MeleeApplyDmgRange; // 0xbf0(0x04)
	float MeleeApplyDmgRangeXY; // 0xbf4(0x04)
	float MeleeApplyDmgAngle; // 0xbf8(0x04)
	float MeleeRange; // 0xbfc(0x04)
	float TimeBetweenMelee; // 0xc00(0x04)
	float MeleeConeHalfAngle; // 0xc04(0x04)
	char pad_C08[0x34]; // 0xc08(0x34)
	float MeleeMaxRotationAngle; // 0xc3c(0x04)
	char pad_C40[0x10]; // 0xc40(0x10)
	struct UDamageType* MeleeDamageType; // 0xc50(0x08)
	char pad_C58[0x10]; // 0xc58(0x10)
	struct UAkAudioEvent* InstantMeleeSpeedReachedEvent; // 0xc68(0x08)
	struct UAkAudioEvent* InstantMeleeSpeedStopEvent; // 0xc70(0x08)
	char pad_C78[0x1]; // 0xc78(0x01)
	char bIsThrusting : 1; // 0xc79(0x01)
	char pad_C79_1 : 7; // 0xc79(0x01)
	char pad_C7A[0x2]; // 0xc7a(0x02)
	float thrustAmountPerTick; // 0xc7c(0x04)
	float thrusterVelocityThreshhold; // 0xc80(0x04)
	float thrusterRechargeDelay; // 0xc84(0x04)
	float thrusterTotalTime; // 0xc88(0x04)
	float thrusterCurrentTime; // 0xc8c(0x04)
	char pad_C90[0x8]; // 0xc90(0x08)
	struct UAkAudioEvent* JetPackStartEvent; // 0xc98(0x08)
	struct UAkAudioEvent* JetPackEndEvent; // 0xca0(0x08)
	char pad_CA8[0x10]; // 0xca8(0x10)
	char bIsClambering : 1; // 0xcb8(0x01)
	char pad_CB8_1 : 7; // 0xcb8(0x01)
	char pad_CB9[0x3]; // 0xcb9(0x03)
	float ClamberCameraShakeDelay; // 0xcbc(0x04)
	struct UCameraShakeBase* ClamberCameraShake; // 0xcc0(0x08)
	char bIsZooming : 1; // 0xcc8(0x01)
	char pad_CC8_1 : 7; // 0xcc8(0x01)
	char pad_CC9[0x3]; // 0xcc9(0x03)
	float maxTimeOutOfBounds; // 0xccc(0x04)
	float curTimeOutOfBounds; // 0xcd0(0x04)
	char pad_CD4[0x4]; // 0xcd4(0x04)
	struct UAkAudioEvent* OutOfBoundsStartEvent; // 0xcd8(0x08)
	struct UAkAudioEvent* OutofBoundsEndEvent; // 0xce0(0x08)
	char pad_CE8[0x8]; // 0xce8(0x08)
	struct UPortalWarsAnimInstanceV2* Animation; // 0xcf0(0x08)
	char pad_CF8[0x20]; // 0xcf8(0x20)
	struct UAkAudioEvent* DeathEvent; // 0xd18(0x08)
	struct UAkAudioEvent* RespawnEvent; // 0xd20(0x08)
	struct UAkAudioEvent* SprayEvent; // 0xd28(0x08)
	struct UAkAudioEvent* Footstep3pEvent; // 0xd30(0x08)
	struct UAkAudioEvent* Footstep1pEvent; // 0xd38(0x08)
	struct UAkAudioEvent* Land3pEvent; // 0xd40(0x08)
	struct UAkAudioEvent* Land1pEvent; // 0xd48(0x08)
	struct UAkAudioEvent* Jump3pEvent; // 0xd50(0x08)
	struct UAkAudioEvent* Jump1pEvent; // 0xd58(0x08)
	struct UPortalWarsAkComponent* AkFirstPerson; // 0xd60(0x08)
	struct UPortalWarsAkComponent* AkThirdPerson; // 0xd68(0x08)
	struct UPortalWarsAkComponent* AkFoot; // 0xd70(0x08)
	struct UPortalWarsAkComponent* AkJetPack; // 0xd78(0x08)
	struct UPortalWarsAkComponent* AkGunLocation; // 0xd80(0x08)
	struct UPortalWarsAkComponent* AkMoveable; // 0xd88(0x08)
	struct UAkAudioEvent* RespawnCountdownEvent; // 0xd90(0x08)
	struct UAkAudioEvent* RespawnCountdownStopEvent; // 0xd98(0x08)
	float FootstepLoudnessForBots; // 0xda0(0x04)
	float ThrusterLoudnessForBots; // 0xda4(0x04)
	char pad_DA8[0x8]; // 0xda8(0x08)
	struct UPortalWarsIndicatorWidget* NameIndicatorWidget; // 0xdb0(0x08)
	struct UPortalWarsRadarMarkerWidget* RadarMarkerWidget; // 0xdb8(0x08)
	struct APortalWarsPlayerState* LastPlayerState; // 0xdc0(0x08)
	char pad_DC8[0x4]; // 0xdc8(0x04)
	uint16_t RemotePitch; // 0xdcc(0x02)
	uint16_t RemoteYaw; // 0xdce(0x02)
	uint16_t RemoteRoll; // 0xdd0(0x02)
	char pad_DD2[0xe]; // 0xdd2(0x0e)
};

// Class PortalWars.Gun
struct AGun {
	char pad_0000[0x2FC]; // 0x0 (0x2FC)
	int32_t CurrentAmmo; // 0x2FC(0x04)
	int32_t CurrentAmmoInClip; // 0x300(0x04)
	char pad_0002[0x14]; // 0x304 (0x14)
	float EquipTime; // 0x318(0x04)
	char pad_0003[0x24]; // 0x31C (0x24)
	FRecoilData recoilConfig; // 0x340(0x18)
};

// Class Engine.Level
struct ULevel : UObject {
	char pad_0000[0x98]; // 0x00 (0x98)
	TArray<AActor*> Actors; // 0x98 (0x10)
	char pad_0001[0x10]; // 0xA8 (0x10)
	class UWorld* OwningWorld; // 0xB8 (0x08)
};

// Class Engine.GameInstance
struct UGameInstance {
	char pad_0000[0x38]; // 0x0 (0x38)
	TArray<class UPlayer*> LocalPlayers; // 0x38(0x10)
};

// Class Engine.Player
struct UPlayer {
	char pad_0000[0x30]; // 0x0 (0x30)
	class APlayerController* PlayerController; // 0x30(0x08)
	char pad_0001[0x38]; // 0x38 (0x38)
	class UGameViewportClient* ViewportClient; // 0x70 (0x08)
};

struct ULocalPlayer : UObject {
	char pad_48[0x28]; // 0x48(0x28)
	struct UGameViewportClient* ViewportClient; // 0x70(0x08)
	char pad_78[0x1c]; // 0x78(0x1c)
	char pad_95[0x3]; // 0x95(0x03)
	struct APlayerController* PendingLevelPlayerControllerClass; // 0x98(0x08)
	char bSentSplitJoin : 1; // 0xa0(0x01)
	char pad_A0_1 : 7; // 0xa0(0x01)
	char pad_A1[0x17]; // 0xa1(0x17)
	int32_t ControllerId; // 0xb8(0x04)
	char pad_BC[0x19c]; // 0xbc(0x19c)
};

// Class PortalWars.PortalWarsLocalPlayer
// Size: 0x5e8 (Inherited: 0x258)
struct UPortalWarsLocalPlayer : ULocalPlayer {
	char pad_258[0x30]; // 0x258(0x30)
	struct UTexture* PlayerAvatar; // 0x288(0x08)
	char pad_290[0x228]; // 0x290(0x228)
	struct TArray<struct UObject*> CachedCustomizationObjects; // 0x4b8(0x10)
	char pad_4C8[0x30]; // 0x4c8(0x30)
	struct UPortalWarsNotificationManager* NotificationManager; // 0x4f8(0x08)
	struct UPortalWarsNotificationManager* NotificationManagerClass; // 0x500(0x08)
	struct UPortalWarsInviteManager* InviteManager; // 0x508(0x08)
	struct UPortalWarsInviteManager* InviteManagerClass; // 0x510(0x08)
	char pad_578[0x38]; // 0x578(0x38)
	struct UPortalWarsSaveGame* UserSaveGameData; // 0x5b0(0x08)
	char pad_5B8[0x30]; // 0x5b8(0x30)

	void SetPlayerAvatar(struct UTexture* avatar); // Function PortalWars.PortalWarsLocalPlayer.SetPlayerAvatar // (Final|Native|Protected) // @ game+0x16a7ef0
	void OnUserCustomizationsUpdate(); // Function PortalWars.PortalWarsLocalPlayer.OnUserCustomizationsUpdate // (Final|Native|Protected) // @ game+0x16a7eb0
	void OnUserChosenCustomizationsUpdate(); // Function PortalWars.PortalWarsLocalPlayer.OnUserChosenCustomizationsUpdate // (Final|Native|Protected) // @ game+0x16a7e90
	void OnCustomizationsLoaded(); // Function PortalWars.PortalWarsLocalPlayer.OnCustomizationsLoaded // (Final|Native|Protected) // @ game+0x16a7b50
	void LoadUserSaveGame(); // Function PortalWars.PortalWarsLocalPlayer.LoadUserSaveGame // (Final|Native|Public) // @ game+0x16a7af0
	struct UPortalWarsSaveGame* GetUserSaveGame(); // Function PortalWars.PortalWarsLocalPlayer.GetUserSaveGame // (Final|Native|Public|Const) // @ game+0x16a7910
};

struct UPortalWarsSaveGame : UObject {
	char pad_28[0xc]; // 0x28(0x0c)
	float FOV; // 0x34(0x04)
	int32_t ColorblindMode; // 0x38(0x04)
	float ColorblindModeIntensity; // 0x3c(0x04)
	float MasterVolume; // 0x40(0x04)
	float MusicVolume; // 0x44(0x04)
	float GameplayVolume; // 0x48(0x04)
	float AnnouncerVolume; // 0x4c(0x04)
	float AmbientVolume; // 0x50(0x04)
	float VOIPVolume; // 0x54(0x04)
	bool bOpenMic; // 0x58(0x01)
	bool bProximityEnabled; // 0x59(0x01)
	char pad_5A[0x6]; // 0x5a(0x06)
	struct FString VoiceChatInputDevice; // 0x60(0x10)
	struct FString VoiceChatOutputDevice; // 0x70(0x10)
	bool bMuteWhileMinimized; // 0x80(0x01)
	bool ToggleCrouch; // 0x81(0x01)
	bool ToggleZoom; // 0x82(0x01)
	bool ToggleSprint; // 0x83(0x01)
	float HorizontalMouseSensitivity; // 0x84(0x04)
	float VerticalMouseSensitivity; // 0x88(0x04)
	float MouseZoomSensitivityMultiplier; // 0x8c(0x04)
	bool MouseInverted; // 0x90(0x01)
	bool MouseAutosprint; // 0x91(0x01)
	bool EnableMouseSmoothing; // 0x92(0x01)
	char pad_93[0x5]; // 0x93(0x05)
	struct TArray<struct FKeyConfigurationInfo> KeyBindings; // 0x98(0x10)
	float HorizontalControllerSensitivity; // 0xa8(0x04)
	float VerticalControllerSensitivity; // 0xac(0x04)
	float ControllerAccelerationSensitivity; // 0xb0(0x04)
	float ControllerZoomSensitivityMultiplier; // 0xb4(0x04)
	float ControllerInnerDeadzone; // 0xb8(0x04)
	float ControllerOuterDeadzone; // 0xbc(0x04)
	float ControllerAimAssistStrength; // 0xc0(0x04)
	bool ControllerInverted; // 0xc4(0x01)
	bool ControllerAutosprint; // 0xc5(0x01)
	bool ControllerTapToInteract; // 0xc6(0x01)
	bool ControllerVibrationEnabled; // 0xc7(0x01)
	int32_t ControllerGameplayPreset; // 0xc8(0x04)
	char pad_CC[0x4]; // 0xcc(0x04)
	char pad_1B8[0x1]; // 0x1b8(0x01)
	bool ShouldShowBlood; // 0x1b9(0x01)
	char pad_1BA[0x2]; // 0x1ba(0x02)
	int32_t PlayKillcamMode; // 0x1bc(0x04)
	bool ShouldPlayKillcam; // 0x1c0(0x01)
	bool ShouldRecordReplay; // 0x1c1(0x01)
	char pad_1C2[0x2]; // 0x1c2(0x02)
	int32_t ClamberMode; // 0x1c4(0x04)
	bool EnableHighSpeedFOVChange; // 0x1c8(0x01)
	char pad_1C9[0x3]; // 0x1c9(0x03)
	int32_t InputDevice; // 0x1cc(0x04)
	struct FLinearColor EnemyColor; // 0x1d0(0x10)
	struct FLinearColor EnemyOutlineColor; // 0x1e0(0x10)
	struct FLinearColor AllyOutlineColor; // 0x1f0(0x10)
	struct FLinearColor AllyColorThroughWalls; // 0x200(0x10)
	bool bShouldAutoSave; // 0x210(0x01)
	bool ShowFPS; // 0x211(0x01)
	bool ShowLatency; // 0x212(0x01)
	bool ShowPing; // 0x213(0x01)
	bool ShowNetInfo; // 0x214(0x01)
	bool ShowSubtitles; // 0x215(0x01)
	bool ShowDamageNumbers; // 0x216(0x01)
	bool ShowLowAmmo; // 0x217(0x01)
	bool ShowSprintCrosshair; // 0x218(0x01)
	char pad_219[0x7]; // 0x219(0x07)
	struct FLinearColor CustomCrosshairColor; // 0x270(0x10)
	struct FLinearColor EnemyCustomCrosshairColor; // 0x280(0x10)
	bool WantsToCrossPlay; // 0x290(0x01)
	bool bAnonymousMode; // 0x291(0x01)
	bool bHideNames; // 0x292(0x01)
	bool bPartyChatOnly; // 0x293(0x01)
	bool bHasCompletedTutorial; // 0x294(0x01)
	char pad_295[0x3]; // 0x295(0x03)
	int32_t TutorialStageID; // 0x298(0x04)
	bool bHasWatchedIntroVideo; // 0x29c(0x01)
	bool bHasSeenBattlePassDialog; // 0x29d(0x01)
	char pad_29E[0x2]; // 0x29e(0x02)
	char pad_2B1[0x7]; // 0x2b1(0x07)
	struct TArray<struct FCustomizationId> ViewedCustomizations; // 0x2b8(0x10)
	bool bHasSeenLocker; // 0x318(0x01)
	char pad_319[0x7]; // 0x319(0x07)
	bool bHasSyncedNewCustomizations; // 0x370(0x01)
	char pad_371[0x7]; // 0x371(0x07)
	struct TArray<struct FStoreItemInfo> ViewedStoreItems; // 0x378(0x10)
	struct TArray<struct FString> ViewedLimitedTimeOffers; // 0x388(0x10)
	struct TArray<struct FStoreItemInfo> LastViewedStore; // 0x398(0x10)
	struct TArray<struct FChallengeData> ViewedChallenges; // 0x3a8(0x10)
	struct TArray<struct FString> MuteList; // 0x3b8(0x10)
	struct FString LoginAuthToken; // 0x3d0(0x10)
	struct FString PreferredMatchmakingRegionName; // 0x3e0(0x10)
	struct FString PreferredCustomRegionName; // 0x3f0(0x10)
	struct FString LastGameVersion; // 0x588(0x10)
	char pad_598[0x28]; // 0x598(0x28)
};

struct FMinimalViewInfo {
	struct FVector Location; // 0x00(0x0c)
	struct FRotator Rotation; // 0x0c(0x0c)
	float FOV; // 0x18(0x04)
	float DesiredFOV; // 0x1c(0x04)
	float OrthoWidth; // 0x20(0x04)
	float OrthoNearClipPlane; // 0x24(0x04)
	float OrthoFarClipPlane; // 0x28(0x04)
	float AspectRatio; // 0x2c(0x04)
	char bConstrainAspectRatio : 1; // 0x30(0x01)
	char bUseFieldOfViewForLOD : 1; // 0x30(0x01)
	char pad_30_2 : 6; // 0x30(0x01)
	char pad_31[0x3]; // 0x31(0x03)
	char ProjectionMode; // 0x34(0x01)
	char pad_35[0x3]; // 0x35(0x03)
	float PostProcessBlendWeight; // 0x38(0x04)
	char pad_3C[0x4]; // 0x3c(0x04)
	char PostProcessSettings[0x560]; // 0x40(0x560)
	struct FVector2D OffCenterProjectionOffset; // 0x5a0(0x08)
	char pad_5A8[0x48]; // 0x5a8(0x48)
};

// ScriptStruct Engine.CameraCacheEntry
// Size: 0x600 (Inherited: 0x00)
struct FCameraCacheEntry {
	float Timestamp; // 0x00(0x04)
	char pad_4[0xc]; // 0x04(0x0c)
	struct FMinimalViewInfo POV; // 0x10(0x5f0)
};

// ScriptStruct Engine.TViewTarget
// Size: 0x610 (Inherited: 0x00)
struct FTViewTarget {
	struct AActor* Target; // 0x00(0x08)
	char pad_8[0x8]; // 0x08(0x08)
	struct FMinimalViewInfo POV; // 0x10(0x5f0)
	struct APlayerState* PlayerState; // 0x600(0x08)
	char pad_608[0x8]; // 0x608(0x08)
};

// Class Engine.PlayerCameraManager
// Size: 0x2810 (Inherited: 0x220)
struct APlayerCameraManager : AActor {
	struct APlayerController* PCOwner; // 0x220(0x08)
	struct USceneComponent* TransformComponent; // 0x228(0x08)
	char pad_230[0x8]; // 0x230(0x08)
	float DefaultFOV; // 0x238(0x04)
	char pad_23C[0x4]; // 0x23c(0x04)
	float DefaultOrthoWidth; // 0x240(0x04)
	char pad_244[0x4]; // 0x244(0x04)
	float DefaultAspectRatio; // 0x248(0x04)
	char pad_24C[0x44]; // 0x24c(0x44)
	struct FCameraCacheEntry CameraCache; // 0x290(0x600)
	struct FCameraCacheEntry LastFrameCameraCache; // 0x890(0x600)
	struct FTViewTarget ViewTarget; // 0xe90(0x610)
	struct FTViewTarget PendingViewTarget; // 0x14a0(0x610)
	char pad_1AB0[0x30]; // 0x1ab0(0x30)
	struct FCameraCacheEntry CameraCachePrivate; // 0x1ae0(0x600)
	struct FCameraCacheEntry LastFrameCameraCachePrivate; // 0x20e0(0x600)
	struct TArray<struct UCameraModifier*> ModifierList; // 0x26e0(0x10)
	struct TArray<struct UCameraModifier*> DefaultModifiers; // 0x26f0(0x10)
	float FreeCamDistance; // 0x2700(0x04)
	struct FVector FreeCamOffset; // 0x2704(0x0c)
	struct FVector ViewTargetOffset; // 0x2710(0x0c)
	char pad_271C[0x4]; // 0x271c(0x04)
	char OnAudioFadeChangeEvent[0x10]; // 0x2720(0x10)
	char pad_2730[0x10]; // 0x2730(0x10)
	struct TArray<struct AEmitterCameraLensEffectBase*> CameraLensEffects; // 0x2740(0x10)
	struct UCameraModifier_CameraShake* CachedCameraShakeMod; // 0x2750(0x08)
	struct UCameraAnimInst* AnimInstPool[0x8]; // 0x2758(0x40)
	struct TArray<struct FPostProcessSettings> PostProcessBlendCache; // 0x2798(0x10)
	char pad_27A8[0x10]; // 0x27a8(0x10)
	struct TArray<struct UCameraAnimInst*> ActiveAnims; // 0x27b8(0x10)
	struct TArray<struct UCameraAnimInst*> FreeAnims; // 0x27c8(0x10)
	struct ACameraActor* AnimCameraActor; // 0x27d8(0x08)
	char bIsOrthographic : 1; // 0x27e0(0x01)
	char bDefaultConstrainAspectRatio : 1; // 0x27e0(0x01)
	char pad_27E0_2 : 4; // 0x27e0(0x01)
	char bClientSimulatingViewTarget : 1; // 0x27e0(0x01)
	char bUseClientSideCameraUpdates : 1; // 0x27e0(0x01)
	char pad_27E1_0 : 2; // 0x27e1(0x01)
	char bGameCameraCutThisFrame : 1; // 0x27e1(0x01)
	char pad_27E1_3 : 5; // 0x27e1(0x01)
	char pad_27E2[0x2]; // 0x27e2(0x02)
	float ViewPitchMin; // 0x27e4(0x04)
	float ViewPitchMax; // 0x27e8(0x04)
	float ViewYawMin; // 0x27ec(0x04)
	float ViewYawMax; // 0x27f0(0x04)
	float ViewRollMin; // 0x27f4(0x04)
	float ViewRollMax; // 0x27f8(0x04)
	char pad_27FC[0x4]; // 0x27fc(0x04)
	float ServerUpdateCameraTimeout; // 0x2800(0x04)
	char pad_2804[0xc]; // 0x2804(0x0c)

	void SwapPendingViewTargetWhenUsingClientSideCameraUpdates(); // Function Engine.PlayerCameraManager.SwapPendingViewTargetWhenUsingClientSideCameraUpdates // (Final|Native|Protected) // @ game+0x37b6620
	void StopCameraShake(struct UCameraShakeBase* ShakeInstance, bool bImmediately); // Function Engine.PlayerCameraManager.StopCameraShake // (Native|Public|BlueprintCallable) // @ game+0x37b64d0
	void StopCameraFade(); // Function Engine.PlayerCameraManager.StopCameraFade // (Native|Public|BlueprintCallable) // @ game+0x37b64b0
	void StopCameraAnimInst(struct UCameraAnimInst* AnimInst, bool bImmediate); // Function Engine.PlayerCameraManager.StopCameraAnimInst // (Native|Public|BlueprintCallable) // @ game+0x37b63e0
	void StopAllInstancesOfCameraShakeFromSource(struct UCameraShakeBase* Shake, struct UCameraShakeSourceComponent* SourceComponent, bool bImmediately); // Function Engine.PlayerCameraManager.StopAllInstancesOfCameraShakeFromSource // (Native|Public|BlueprintCallable) // @ game+0x37b62d0
	void StopAllInstancesOfCameraShake(struct UCameraShakeBase* Shake, bool bImmediately); // Function Engine.PlayerCameraManager.StopAllInstancesOfCameraShake // (Native|Public|BlueprintCallable) // @ game+0x37b6200
	void StopAllInstancesOfCameraAnim(struct UCameraAnim* Anim, bool bImmediate); // Function Engine.PlayerCameraManager.StopAllInstancesOfCameraAnim // (Native|Public|BlueprintCallable) // @ game+0x37b6130
	void StopAllCameraShakesFromSource(struct UCameraShakeSourceComponent* SourceComponent, bool bImmediately); // Function Engine.PlayerCameraManager.StopAllCameraShakesFromSource // (Native|Public|BlueprintCallable) // @ game+0x37b6060
	void StopAllCameraShakes(bool bImmediately); // Function Engine.PlayerCameraManager.StopAllCameraShakes // (Native|Public|BlueprintCallable) // @ game+0x37b5fd0
	void StopAllCameraAnims(bool bImmediate); // Function Engine.PlayerCameraManager.StopAllCameraAnims // (Native|Public|BlueprintCallable) // @ game+0x37b5f40
	struct UCameraShakeBase* StartCameraShakeFromSource(struct UCameraShakeBase* ShakeClass, struct UCameraShakeSourceComponent* SourceComponent, float Scale, enum class ECameraShakePlaySpace PlaySpace, struct FRotator UserPlaySpaceRot); // Function Engine.PlayerCameraManager.StartCameraShakeFromSource // (Native|Public|HasDefaults|BlueprintCallable) // @ game+0x37b5cd0
	struct UCameraShakeBase* StartCameraShake(struct UCameraShakeBase* ShakeClass, float Scale, enum class ECameraShakePlaySpace PlaySpace, struct FRotator UserPlaySpaceRot); // Function Engine.PlayerCameraManager.StartCameraShake // (Native|Public|HasDefaults|BlueprintCallable) // @ game+0x37b5b50
	void StartCameraFade(float FromAlpha, float ToAlpha, float Duration, struct FLinearColor Color, bool bShouldFadeAudio, bool bHoldWhenFinished); // Function Engine.PlayerCameraManager.StartCameraFade // (Native|Public|HasDefaults|BlueprintCallable) // @ game+0x37b5960
	void SetManualCameraFade(float InFadeAmount, struct FLinearColor Color, bool bInFadeAudio); // Function Engine.PlayerCameraManager.SetManualCameraFade // (Native|Public|HasDefaults|BlueprintCallable) // @ game+0x37b53c0
	void SetGameCameraCutThisFrame(); // Function Engine.PlayerCameraManager.SetGameCameraCutThisFrame // (Final|Native|Public|BlueprintCallable) // @ game+0x37b5290
	bool RemoveCameraModifier(struct UCameraModifier* ModifierToRemove); // Function Engine.PlayerCameraManager.RemoveCameraModifier // (Native|Public|BlueprintCallable) // @ game+0x37b3c80
	void RemoveCameraLensEffect(struct AEmitterCameraLensEffectBase* Emitter); // Function Engine.PlayerCameraManager.RemoveCameraLensEffect // (Native|Public|BlueprintCallable) // @ game+0x1669390
	struct UCameraAnimInst* PlayCameraAnim(struct UCameraAnim* Anim, float Rate, float Scale, float BlendInTime, float BlendOutTime, bool bLoop, bool bRandomStartTime, float Duration, enum class ECameraShakePlaySpace PlaySpace, struct FRotator UserPlaySpaceRot); // Function Engine.PlayerCameraManager.PlayCameraAnim // (Native|Public|HasDefaults|BlueprintCallable) // @ game+0x37b3420
	void PhotographyCameraModify(struct FVector NewCameraLocation, struct FVector PreviousCameraLocation, struct FVector OriginalCameraLocation, struct FVector& ResultCameraLocation); // Function Engine.PlayerCameraManager.PhotographyCameraModify // (BlueprintCosmetic|Native|Event|Public|HasOutParms|HasDefaults|BlueprintEvent) // @ game+0x37b3290
	void OnPhotographySessionStart(); // Function Engine.PlayerCameraManager.OnPhotographySessionStart // (BlueprintCosmetic|Native|Event|Public|BlueprintEvent) // @ game+0x165c3b0
	void OnPhotographySessionEnd(); // Function Engine.PlayerCameraManager.OnPhotographySessionEnd // (BlueprintCosmetic|Native|Event|Public|BlueprintEvent) // @ game+0x16f3390
	void OnPhotographyMultiPartCaptureStart(); // Function Engine.PlayerCameraManager.OnPhotographyMultiPartCaptureStart // (BlueprintCosmetic|Native|Event|Public|BlueprintEvent) // @ game+0x16a2a70
	void OnPhotographyMultiPartCaptureEnd(); // Function Engine.PlayerCameraManager.OnPhotographyMultiPartCaptureEnd // (BlueprintCosmetic|Native|Event|Public|BlueprintEvent) // @ game+0x164ce00
	struct APlayerController* GetOwningPlayerController(); // Function Engine.PlayerCameraManager.GetOwningPlayerController // (Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x37b2d60
	float GetFOVAngle(); // Function Engine.PlayerCameraManager.GetFOVAngle // (Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x36c9d80
	struct FRotator GetCameraRotation(); // Function Engine.PlayerCameraManager.GetCameraRotation // (Native|Public|HasDefaults|BlueprintCallable|BlueprintPure|Const) // @ game+0x37b19a0
	struct FVector GetCameraLocation(); // Function Engine.PlayerCameraManager.GetCameraLocation // (Native|Public|HasDefaults|BlueprintCallable|BlueprintPure|Const) // @ game+0x37b1960
	struct UCameraModifier* FindCameraModifierByClass(struct UCameraModifier* ModifierClass); // Function Engine.PlayerCameraManager.FindCameraModifierByClass // (Native|Public|BlueprintCallable) // @ game+0x37b18c0
	void ClearCameraLensEffects(); // Function Engine.PlayerCameraManager.ClearCameraLensEffects // (Native|Public|BlueprintCallable) // @ game+0x16a3ab0
	bool BlueprintUpdateCamera(struct AActor* CameraTarget, struct FVector& NewCameraLocation, struct FRotator& NewCameraRotation, float& NewCameraFOV); // Function Engine.PlayerCameraManager.BlueprintUpdateCamera // (BlueprintCosmetic|Event|Public|HasOutParms|HasDefaults|BlueprintEvent) // @ game+0x1a5c6b0
	struct UCameraModifier* AddNewCameraModifier(struct UCameraModifier* ModifierClass); // Function Engine.PlayerCameraManager.AddNewCameraModifier // (Native|Public|BlueprintCallable) // @ game+0x37ae5e0
	struct AEmitterCameraLensEffectBase* AddCameraLensEffect(struct AEmitterCameraLensEffectBase* LensEffectEmitterClass); // Function Engine.PlayerCameraManager.AddCameraLensEffect // (Native|Public|BlueprintCallable) // @ game+0x37ae540
	void SetCameraCachePOV(const FMinimalViewInfo& InPOV);
};

// Class Engine.PlayerState
struct APlayerState {
	char pad_0000[0x280]; // 0x0 (0x280)
	class APawn* PawnPrivate; // 0x280 (0x08)
	char pad_0001[0x78]; // 0x228 (0x78)
	FString PlayerNamePrivate; // 0x300 (0x10)
	char pad_0002[0x28]; // 0x310 (0x28)
	BYTE TeamNum; // 0x338 (0x01)
};

// ScriptStruct Engine.HitResult
// Size: 0x88 (Inherited: 0x00)
struct FHitResult {
	int32_t FaceIndex; // 0x00(0x04)
	float Time; // 0x04(0x04)
	float Distance; // 0x08(0x04)
	char Location[0x0c]; // 0x0c(0x0c)
	char ImpactPoint[0x0c]; // 0x18(0x0c)
	char Normal[0x0c]; // 0x24(0x0c)
	char ImpactNormal[0x0c]; // 0x30(0x0c)
	char TraceStart[0x0c]; // 0x3c(0x0c)
	char TraceEnd[0x0c]; // 0x48(0x0c)
	float PenetrationDepth; // 0x54(0x04)
	int32_t Item; // 0x58(0x04)
	char ElementIndex; // 0x5c(0x01)
	char bBlockingHit : 1; // 0x5d(0x01)
	char bStartPenetrating : 1; // 0x5d(0x01)
	char pad_5D_2 : 6; // 0x5d(0x01)
	char pad_5E[0x2]; // 0x5e(0x02)
	char PhysMaterial[0x08]; // 0x60(0x08)
	char Actor[0x08]; // 0x68(0x08)
	char Component[0x08]; // 0x70(0x08)
	struct FName BoneName; // 0x78(0x08)
	struct FName MyBoneName; // 0x80(0x08)
};

// Class Engine.SkinnedMeshComponent
struct USkeletalMeshComponent : UObject {
	char pad_0000[0x00]; // 0x28
	FName GetBoneName(INT BoneIndex);
	FVector GetBoneMatrix(INT index);
	void K2_SetRelativeRotation(FRotator NewRotation, bool bSweep, bool bTeleport);
};

// Class Engine.SceneComponent
struct USceneComponent {
	char pad_0000[0x11C]; // 0x0 (0x11C)
	struct FVector RelativeLocation; // 0x11C(0x0C)
	struct FRotator RelativeRotation; // 0x128(0x0C)
};

struct ULevelStreaming : UObject {
	FName PackageNameToLoad;
};

// Class Engine.World 
struct UWorld {
	char pad_0000[0x30]; // 0x0 (0x30)
	class ULevel* PersistentLevel; // 0x30(0x08)
	char pad_0001[0x148]; // 0x38 (0x148)
	class UGameInstance* OwningGameInstance; // 0x180 (0x08)
	TArray<ULevel*> Levels;
	TArray<ULevelStreaming*> StreamingLevels;
	void** VFTable;
};

// Class Engine.Canvas
struct Canvas : UObject {
	char pad_0000[0x00]; // 0x28
	void K2_DrawLine(FVector2D ScreenPositionA, FVector2D ScreenPositionB, FLOAT Thickness, FLinearColor Color);
	void K2_DrawText(FString RenderText, FVector2D ScreenPosition, FVector2D Scale, FLinearColor RenderColor, float Kerning, FLinearColor ShadowColor, FVector2D ShadowOffset, bool bCentreX, bool bCentreY, bool bOutlined, FLinearColor OutlineColor);
};

// Class Engine.GameViewportClient
struct UGameViewportClient : UObject {
	char pad_0000[0x00]; // 0x28 
};

extern FNamePool* NamePoolData;
extern TUObjectArray* ObjObjects;
extern UWorld* WRLD;

bool EngineInit();