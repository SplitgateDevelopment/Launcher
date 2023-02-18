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
	class AGun* CurrentWeapon; // 0x7f0 (0x08)
	float SprintingSpeedModifier;
	float MeleeRange;
	float TimeBetweenMelee;
	float MeleeApplyDmgAngle;
	float MeleeApplyDmgRange;
	float MeleeApplyDmgRangeXY;
	float thrustAmountPerTick;
	float thrusterVelocityThreshhold;
	float thrusterRechargeDelay;
	float thrusterTotalTime;
	float curTimeOutOfBounds;
	float maxTimeOutOfBounds;
	float SprayRange;
	float SprayLifetime;
	float TimeBetweenSprays;
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

// Class Engine.PlayerState
struct APlayerState {
	char pad_0000[0x280]; // 0x0 (0x280)
	class APawn* PawnPrivate; // 0x280 (0x08)
	char pad_0001[0x78]; // 0x228 (0x78)
	FString PlayerNamePrivate; // 0x300 (0x10)
	char pad_0002[0x28]; // 0x310 (0x28)
	BYTE TeamNum; // 0x338 (0x01)
};

// Class Engine.SkinnedMeshComponent
struct USkeletalMeshComponent : UObject {
	char pad_0000[0x00]; // 0x28
	FName GetBoneName(INT BoneIndex);
	FVector GetBoneMatrix(INT index);
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

struct APlayerCameraManager {
	float DefaultFOV;
};

extern FNamePool* NamePoolData;
extern TUObjectArray* ObjObjects;
extern UWorld* WRLD;
extern void(*OPostRender)(UGameViewportClient* UGameViewportClient, Canvas* Canvas);

bool EngineInit();