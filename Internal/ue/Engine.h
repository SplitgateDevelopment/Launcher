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
	bool IsDefaultObject() const;
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

// ScriptStruct CoreUObject.Color
// Size: 0x04 (Inherited: 0x00)
struct FColor {
	char B; // 0x00(0x01)
	char G; // 0x01(0x01)
	char R; // 0x02(0x01)
	char A; // 0x03(0x01)
};

// ScriptStruct CoreUObject.LinearColor
// Size: 0x10 (Inherited: 0x00)
struct FLinearColor {
	float R; // 0x00(0x04)
	float G; // 0x04(0x04)
	float B; // 0x08(0x04)
	float A; // 0x0c(0x04)
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

struct FMulticastInlineDelegate {
	char pad_0[0x10];
};

// Class Engine.Actor
// Size: 0x220 (Inherited: 0x28)
struct AActor : UObject {
	char PrimaryActorTick[0x30]; // 0x28(0x30)
	char bNetTemporary : 1; // 0x58(0x01)
	char bNetStartup : 1; // 0x58(0x01)
	char bOnlyRelevantToOwner : 1; // 0x58(0x01)
	char bAlwaysRelevant : 1; // 0x58(0x01)
	char bReplicateMovement : 1; // 0x58(0x01)
	char bHidden : 1; // 0x58(0x01)
	char bTearOff : 1; // 0x58(0x01)
	char bForceNetAddressable : 1; // 0x58(0x01)
	char bExchangedRoles : 1; // 0x59(0x01)
	char bNetLoadOnClient : 1; // 0x59(0x01)
	char bNetUseOwnerRelevancy : 1; // 0x59(0x01)
	char bRelevantForNetworkReplays : 1; // 0x59(0x01)
	char bRelevantForLevelBounds : 1; // 0x59(0x01)
	char bReplayRewindable : 1; // 0x59(0x01)
	char bAllowTickBeforeBeginPlay : 1; // 0x59(0x01)
	char bAutoDestroyWhenFinished : 1; // 0x59(0x01)
	char bCanBeDamaged : 1; // 0x5a(0x01)
	char bBlockInput : 1; // 0x5a(0x01)
	char bCollideWhenPlacing : 1; // 0x5a(0x01)
	char bFindCameraComponentWhenViewTarget : 1; // 0x5a(0x01)
	char bGenerateOverlapEventsDuringLevelStreaming : 1; // 0x5a(0x01)
	char bIgnoresOriginShifting : 1; // 0x5a(0x01)
	char bEnableAutoLODGeneration : 1; // 0x5a(0x01)
	char bIsEditorOnlyActor : 1; // 0x5a(0x01)
	char bActorSeamlessTraveled : 1; // 0x5b(0x01)
	char bReplicates : 1; // 0x5b(0x01)
	char bCanBeInCluster : 1; // 0x5b(0x01)
	char bAllowReceiveTickEventOnDedicatedServer : 1; // 0x5b(0x01)
	char pad_5B_4 : 4; // 0x5b(0x01)
	char pad_5C_0 : 3; // 0x5c(0x01)
	char bActorEnableCollision : 1; // 0x5c(0x01)
	char bActorIsBeingDestroyed : 1; // 0x5c(0x01)
	char pad_5C_5 : 3; // 0x5c(0x01)
	char UpdateOverlapsMethodDuringLevelStreaming[0x01]; // 0x5d(0x01)
	char DefaultUpdateOverlapsMethodDuringLevelStreaming[0x01]; // 0x5e(0x01)
	char RemoteRole[0x01]; // 0x5f(0x01)
	char ReplicatedMovement[0x34]; // 0x60(0x34)
	float InitialLifeSpan; // 0x94(0x04)
	float CustomTimeDilation; // 0x98(0x04)
	char pad_9C[0x4]; // 0x9c(0x04)
	char AttachmentReplication[0x40]; // 0xa0(0x40)
	struct AActor* Owner; // 0xe0(0x08)
	struct FName NetDriverName; // 0xe8(0x08)
	char Role[0x01]; // 0xf0(0x01)
	char NetDormancy[0x01]; // 0xf1(0x01)
	char SpawnCollisionHandlingMethod[0x01]; // 0xf2(0x01)
	char AutoReceiveInput[0x01]; // 0xf3(0x01)
	int32_t InputPriority; // 0xf4(0x04)
	struct UInputComponent* InputComponent; // 0xf8(0x08)
	float NetCullDistanceSquared; // 0x100(0x04)
	int32_t NetTag; // 0x104(0x04)
	float NetUpdateFrequency; // 0x108(0x04)
	float MinNetUpdateFrequency; // 0x10c(0x04)
	float NetPriority; // 0x110(0x04)
	char pad_114[0x4]; // 0x114(0x04)
	struct APawn* Instigator; // 0x118(0x08)
	struct TArray<struct AActor*> Children; // 0x120(0x10)
	struct USceneComponent* RootComponent; // 0x130(0x08)
	struct TArray<struct AMatineeActor*> ControllingMatineeActors; // 0x138(0x10)
	char pad_148[0x8]; // 0x148(0x08)
	struct TArray<struct FName> Layers; // 0x150(0x10)
	char ParentComponent[0x08]; // 0x160(0x08)
	char pad_168[0x8]; // 0x168(0x08)
	struct TArray<struct FName> Tags; // 0x170(0x10)
	char OnTakeAnyDamage[0x01]; // 0x180(0x01)
	char OnTakePointDamage[0x01]; // 0x181(0x01)
	char OnTakeRadialDamage[0x01]; // 0x182(0x01)
	char OnActorBeginOverlap[0x01]; // 0x183(0x01)
	char OnActorEndOverlap[0x01]; // 0x184(0x01)
	char OnBeginCursorOver[0x01]; // 0x185(0x01)
	char OnEndCursorOver[0x01]; // 0x186(0x01)
	char OnClicked[0x01]; // 0x187(0x01)
	char OnReleased[0x01]; // 0x188(0x01)
	char OnInputTouchBegin[0x01]; // 0x189(0x01)
	char OnInputTouchEnd[0x01]; // 0x18a(0x01)
	char OnInputTouchEnter[0x01]; // 0x18b(0x01)
	char OnInputTouchLeave[0x01]; // 0x18c(0x01)
	char OnActorHit[0x01]; // 0x18d(0x01)
	char OnDestroyed[0x01]; // 0x18e(0x01)
	char OnEndPlay[0x01]; // 0x18f(0x01)
	char pad_190[0x60]; // 0x190(0x60)
	struct TArray<struct UActorComponent*> InstanceComponents; // 0x1f0(0x10)
	struct TArray<struct UActorComponent*> BlueprintCreatedComponents; // 0x200(0x10)
	char pad_210[0x10]; // 0x210(0x10)

	bool WasRecentlyRendered(float Tolerance); // Function Engine.Actor.WasRecentlyRendered // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x368d6b0
	void UserConstructionScript(); // Function Engine.Actor.UserConstructionScript // (Event|Public|BlueprintEvent) // @ game+0x1a5c6b0
	void TearOff(); // Function Engine.Actor.TearOff // (Native|Public|BlueprintCallable) // @ game+0x27d8180
	void SnapRootComponentTo(struct AActor* InParentActor, struct FName InSocketName); // Function Engine.Actor.SnapRootComponentTo // (Final|Native|Public|BlueprintCallable) // @ game+0x368d5e0
	void SetTickGroup(enum class ETickingGroup NewTickGroup); // Function Engine.Actor.SetTickGroup // (Final|Native|Public|BlueprintCallable) // @ game+0x368d4d0
	void SetTickableWhenPaused(bool bTickableWhenPaused); // Function Engine.Actor.SetTickableWhenPaused // (Final|Native|Public|BlueprintCallable) // @ game+0x368d550
	void SetReplicates(bool bInReplicates); // Function Engine.Actor.SetReplicates // (Final|BlueprintAuthorityOnly|Native|Public|BlueprintCallable) // @ game+0x368d440
	void SetReplicateMovement(bool bInReplicateMovement); // Function Engine.Actor.SetReplicateMovement // (Native|Public|BlueprintCallable) // @ game+0x368d3b0
	void SetOwner(struct AActor* NewOwner); // Function Engine.Actor.SetOwner // (Native|Public|BlueprintCallable) // @ game+0x368d320
	void SetNetDormancy(enum class ENetDormancy NewDormancy); // Function Engine.Actor.SetNetDormancy // (Final|BlueprintAuthorityOnly|Native|Public|BlueprintCallable) // @ game+0x368d2a0
	void SetLifeSpan(float InLifespan); // Function Engine.Actor.SetLifeSpan // (Native|Public|BlueprintCallable) // @ game+0x368d210
	void SetAutoDestroyWhenFinished(bool bVal); // Function Engine.Actor.SetAutoDestroyWhenFinished // (Final|Native|Public|BlueprintCallable) // @ game+0x368d180
	void SetActorTickInterval(float TickInterval); // Function Engine.Actor.SetActorTickInterval // (Final|Native|Public|BlueprintCallable) // @ game+0x368d100
	void SetActorTickEnabled(bool bEnabled); // Function Engine.Actor.SetActorTickEnabled // (Final|Native|Public|BlueprintCallable) // @ game+0x368d070
	void SetActorScale3D(struct FVector NewScale3D); // Function Engine.Actor.SetActorScale3D // (Final|Native|Public|HasDefaults|BlueprintCallable) // @ game+0x368cfe0
	void SetActorRelativeScale3D(struct FVector NewRelativeScale); // Function Engine.Actor.SetActorRelativeScale3D // (Final|Native|Public|HasDefaults|BlueprintCallable) // @ game+0x368cf50
	void SetActorHiddenInGame(bool bNewHidden); // Function Engine.Actor.SetActorHiddenInGame // (Native|Public|BlueprintCallable) // @ game+0x368cec0
	void SetActorEnableCollision(bool bNewActorEnableCollision); // Function Engine.Actor.SetActorEnableCollision // (Final|Native|Public|BlueprintCallable) // @ game+0x368ce30
	void RemoveTickPrerequisiteComponent(struct UActorComponent* PrerequisiteComponent); // Function Engine.Actor.RemoveTickPrerequisiteComponent // (Native|Public|BlueprintCallable) // @ game+0x368cda0
	void RemoveTickPrerequisiteActor(struct AActor* PrerequisiteActor); // Function Engine.Actor.RemoveTickPrerequisiteActor // (Native|Public|BlueprintCallable) // @ game+0x368cd10
	void ReceiveTick(float DeltaSeconds); // Function Engine.Actor.ReceiveTick // (Event|Public|BlueprintEvent) // @ game+0x1a5c6b0
	void ReceiveRadialDamage(float DamageReceived, struct UDamageType* DamageType, struct FVector Origin, struct FHitResult& HitInfo, struct AController* InstigatedBy, struct AActor* DamageCauser); // Function Engine.Actor.ReceiveRadialDamage // (BlueprintAuthorityOnly|Event|Public|HasOutParms|HasDefaults|BlueprintEvent) // @ game+0x1a5c6b0
	void ReceivePointDamage(float Damage, struct UDamageType* DamageType, struct FVector HitLocation, struct FVector HitNormal, struct UPrimitiveComponent* HitComponent, struct FName BoneName, struct FVector ShotFromDirection, struct AController* InstigatedBy, struct AActor* DamageCauser, struct FHitResult& HitInfo); // Function Engine.Actor.ReceivePointDamage // (BlueprintAuthorityOnly|Event|Public|HasOutParms|HasDefaults|BlueprintEvent) // @ game+0x1a5c6b0
	void ReceiveHit(struct UPrimitiveComponent* MyComp, struct AActor* Other, struct UPrimitiveComponent* OtherComp, bool bSelfMoved, struct FVector HitLocation, struct FVector HitNormal, struct FVector NormalImpulse, struct FHitResult& Hit); // Function Engine.Actor.ReceiveHit // (Event|Public|HasOutParms|HasDefaults|BlueprintEvent) // @ game+0x1a5c6b0
	void ReceiveEndPlay(enum class EEndPlayReason EndPlayReason); // Function Engine.Actor.ReceiveEndPlay // (Event|Protected|BlueprintEvent) // @ game+0x1a5c6b0
	void ReceiveDestroyed(); // Function Engine.Actor.ReceiveDestroyed // (Event|Public|BlueprintEvent) // @ game+0x1a5c6b0
	void ReceiveBeginPlay(); // Function Engine.Actor.ReceiveBeginPlay // (Event|Protected|BlueprintEvent) // @ game+0x1a5c6b0
	void ReceiveAnyDamage(float Damage, struct UDamageType* DamageType, struct AController* InstigatedBy, struct AActor* DamageCauser); // Function Engine.Actor.ReceiveAnyDamage // (BlueprintAuthorityOnly|Event|Public|BlueprintEvent) // @ game+0x1a5c6b0
	void ReceiveActorOnReleased(struct FKey ButtonReleased); // Function Engine.Actor.ReceiveActorOnReleased // (Event|Public|BlueprintEvent) // @ game+0x1a5c6b0
	void ReceiveActorOnInputTouchLeave(enum class ETouchIndex FingerIndex); // Function Engine.Actor.ReceiveActorOnInputTouchLeave // (Event|Public|BlueprintEvent) // @ game+0x1a5c6b0
	void ReceiveActorOnInputTouchEnter(enum class ETouchIndex FingerIndex); // Function Engine.Actor.ReceiveActorOnInputTouchEnter // (Event|Public|BlueprintEvent) // @ game+0x1a5c6b0
	void ReceiveActorOnInputTouchEnd(enum class ETouchIndex FingerIndex); // Function Engine.Actor.ReceiveActorOnInputTouchEnd // (Event|Public|BlueprintEvent) // @ game+0x1a5c6b0
	void ReceiveActorOnInputTouchBegin(enum class ETouchIndex FingerIndex); // Function Engine.Actor.ReceiveActorOnInputTouchBegin // (Event|Public|BlueprintEvent) // @ game+0x1a5c6b0
	void ReceiveActorOnClicked(struct FKey ButtonPressed); // Function Engine.Actor.ReceiveActorOnClicked // (Event|Public|BlueprintEvent) // @ game+0x1a5c6b0
	void ReceiveActorEndOverlap(struct AActor* OtherActor); // Function Engine.Actor.ReceiveActorEndOverlap // (Event|Public|BlueprintEvent) // @ game+0x1a5c6b0
	void ReceiveActorEndCursorOver(); // Function Engine.Actor.ReceiveActorEndCursorOver // (Event|Public|BlueprintEvent) // @ game+0x1a5c6b0
	void ReceiveActorBeginOverlap(struct AActor* OtherActor); // Function Engine.Actor.ReceiveActorBeginOverlap // (Event|Public|BlueprintEvent) // @ game+0x1a5c6b0
	void ReceiveActorBeginCursorOver(); // Function Engine.Actor.ReceiveActorBeginCursorOver // (Event|Public|BlueprintEvent) // @ game+0x1a5c6b0
	void PrestreamTextures(float Seconds, bool bEnableStreaming, int32_t CinematicTextureGroups); // Function Engine.Actor.PrestreamTextures // (Native|Public|BlueprintCallable) // @ game+0x368cbf0
	void OnRep_ReplicateMovement(); // Function Engine.Actor.OnRep_ReplicateMovement // (Native|Public) // @ game+0x102e110
	void OnRep_ReplicatedMovement(); // Function Engine.Actor.OnRep_ReplicatedMovement // (Native|Public) // @ game+0x368cbd0
	void OnRep_Owner(); // Function Engine.Actor.OnRep_Owner // (Native|Protected) // @ game+0x102df70
	void OnRep_Instigator(); // Function Engine.Actor.OnRep_Instigator // (Native|Public) // @ game+0x368cbb0
	void OnRep_AttachmentReplication(); // Function Engine.Actor.OnRep_AttachmentReplication // (Native|Public) // @ game+0x1034d00
	void MakeNoise(float Loudness, struct APawn* NoiseInstigator, struct FVector NoiseLocation, float MaxRange, struct FName Tag); // Function Engine.Actor.MakeNoise // (Final|BlueprintAuthorityOnly|Native|Public|HasDefaults|BlueprintCallable) // @ game+0x368ca10
	struct UMaterialInstanceDynamic* MakeMIDForMaterial(struct UMaterialInterface* Parent); // Function Engine.Actor.MakeMIDForMaterial // (Final|Native|Public|BlueprintCallable) // @ game+0x368c970
	bool K2_TeleportTo(struct FVector DestLocation, struct FRotator DestRotation); // Function Engine.Actor.K2_TeleportTo // (Final|Native|Public|HasDefaults|BlueprintCallable) // @ game+0x368c880
	bool K2_SetActorTransform(struct FTransform& NewTransform, bool bSweep, struct FHitResult& SweepHitResult, bool bTeleport); // Function Engine.Actor.K2_SetActorTransform // (Final|Native|Public|HasOutParms|HasDefaults|BlueprintCallable) // @ game+0x368c670
	bool K2_SetActorRotation(struct FRotator NewRotation, bool bTeleportPhysics); // Function Engine.Actor.K2_SetActorRotation // (Final|Native|Public|HasDefaults|BlueprintCallable) // @ game+0x368c580
	void K2_SetActorRelativeTransform(struct FTransform& NewRelativeTransform, bool bSweep, struct FHitResult& SweepHitResult, bool bTeleport); // Function Engine.Actor.K2_SetActorRelativeTransform // (Final|Native|Public|HasOutParms|HasDefaults|BlueprintCallable) // @ game+0x368c370
	void K2_SetActorRelativeRotation(struct FRotator NewRelativeRotation, bool bSweep, struct FHitResult& SweepHitResult, bool bTeleport); // Function Engine.Actor.K2_SetActorRelativeRotation // (Final|Native|Public|HasOutParms|HasDefaults|BlueprintCallable) // @ game+0x368c1c0
	void K2_SetActorRelativeLocation(struct FVector NewRelativeLocation, bool bSweep, struct FHitResult& SweepHitResult, bool bTeleport); // Function Engine.Actor.K2_SetActorRelativeLocation // (Final|Native|Public|HasOutParms|HasDefaults|BlueprintCallable) // @ game+0x368c010
	bool K2_SetActorLocationAndRotation(struct FVector NewLocation, struct FRotator NewRotation, bool bSweep, struct FHitResult& SweepHitResult, bool bTeleport); // Function Engine.Actor.K2_SetActorLocationAndRotation // (Final|Native|Public|HasOutParms|HasDefaults|BlueprintCallable) // @ game+0x368be00
	bool K2_SetActorLocation(struct FVector NewLocation, bool bSweep, struct FHitResult& SweepHitResult, bool bTeleport); // Function Engine.Actor.K2_SetActorLocation // (Final|Native|Public|HasOutParms|HasDefaults|BlueprintCallable) // @ game+0x368bc40
	void K2_OnReset(); // Function Engine.Actor.K2_OnReset // (Event|Public|BlueprintEvent) // @ game+0x1a5c6b0
	void K2_OnEndViewTarget(struct APlayerController* PC); // Function Engine.Actor.K2_OnEndViewTarget // (Event|Public|BlueprintEvent) // @ game+0x1a5c6b0
	void K2_OnBecomeViewTarget(struct APlayerController* PC); // Function Engine.Actor.K2_OnBecomeViewTarget // (Event|Public|BlueprintEvent) // @ game+0x1a5c6b0
	struct USceneComponent* K2_GetRootComponent(); // Function Engine.Actor.K2_GetRootComponent // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x368bc20
	struct TArray<struct UActorComponent*> K2_GetComponentsByClass(struct UActorComponent* ComponentClass); // Function Engine.Actor.K2_GetComponentsByClass // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x368bb40
	struct FRotator K2_GetActorRotation(); // Function Engine.Actor.K2_GetActorRotation // (Final|Native|Public|HasDefaults|BlueprintCallable|BlueprintPure|Const) // @ game+0x368ba80
	struct FVector K2_GetActorLocation(); // Function Engine.Actor.K2_GetActorLocation // (Final|Native|Public|HasDefaults|BlueprintCallable|BlueprintPure|Const) // @ game+0x368ba00
	void K2_DetachFromActor(enum class EDetachmentRule LocationRule, enum class EDetachmentRule RotationRule, enum class EDetachmentRule ScaleRule); // Function Engine.Actor.K2_DetachFromActor // (Final|Native|Public|BlueprintCallable) // @ game+0x368b8f0
	void K2_DestroyComponent(struct UActorComponent* Component); // Function Engine.Actor.K2_DestroyComponent // (Final|Native|Public|BlueprintCallable) // @ game+0x368b860
	void K2_DestroyActor(); // Function Engine.Actor.K2_DestroyActor // (Native|Public|BlueprintCallable) // @ game+0x368b840
	void K2_AttachToComponent(struct USceneComponent* Parent, struct FName SocketName, enum class EAttachmentRule LocationRule, enum class EAttachmentRule RotationRule, enum class EAttachmentRule ScaleRule, bool bWeldSimulatedBodies); // Function Engine.Actor.K2_AttachToComponent // (Final|Native|Public|BlueprintCallable) // @ game+0x368b670
	void K2_AttachToActor(struct AActor* ParentActor, struct FName SocketName, enum class EAttachmentRule LocationRule, enum class EAttachmentRule RotationRule, enum class EAttachmentRule ScaleRule, bool bWeldSimulatedBodies); // Function Engine.Actor.K2_AttachToActor // (Final|Native|Public|BlueprintCallable) // @ game+0x368b4a0
	void K2_AttachRootComponentToActor(struct AActor* InParentActor, struct FName InSocketName, enum class EAttachLocation AttachLocationType, bool bWeldSimulatedBodies); // Function Engine.Actor.K2_AttachRootComponentToActor // (Final|Native|Public|BlueprintCallable) // @ game+0x368b340
	void K2_AttachRootComponentTo(struct USceneComponent* InParent, struct FName InSocketName, enum class EAttachLocation AttachLocationType, bool bWeldSimulatedBodies); // Function Engine.Actor.K2_AttachRootComponentTo // (Final|Native|Public|BlueprintCallable) // @ game+0x368b1e0
	void K2_AddActorWorldTransformKeepScale(struct FTransform& DeltaTransform, bool bSweep, struct FHitResult& SweepHitResult, bool bTeleport); // Function Engine.Actor.K2_AddActorWorldTransformKeepScale // (Final|Native|Public|HasOutParms|HasDefaults|BlueprintCallable) // @ game+0x368afd0
	void K2_AddActorWorldTransform(struct FTransform& DeltaTransform, bool bSweep, struct FHitResult& SweepHitResult, bool bTeleport); // Function Engine.Actor.K2_AddActorWorldTransform // (Final|Native|Public|HasOutParms|HasDefaults|BlueprintCallable) // @ game+0x368adc0
	void K2_AddActorWorldRotation(struct FRotator DeltaRotation, bool bSweep, struct FHitResult& SweepHitResult, bool bTeleport); // Function Engine.Actor.K2_AddActorWorldRotation // (Final|Native|Public|HasOutParms|HasDefaults|BlueprintCallable) // @ game+0x368ac10
	void K2_AddActorWorldOffset(struct FVector DeltaLocation, bool bSweep, struct FHitResult& SweepHitResult, bool bTeleport); // Function Engine.Actor.K2_AddActorWorldOffset // (Final|Native|Public|HasOutParms|HasDefaults|BlueprintCallable) // @ game+0x368aa60
	void K2_AddActorLocalTransform(struct FTransform& NewTransform, bool bSweep, struct FHitResult& SweepHitResult, bool bTeleport); // Function Engine.Actor.K2_AddActorLocalTransform // (Final|Native|Public|HasOutParms|HasDefaults|BlueprintCallable) // @ game+0x368a850
	void K2_AddActorLocalRotation(struct FRotator DeltaRotation, bool bSweep, struct FHitResult& SweepHitResult, bool bTeleport); // Function Engine.Actor.K2_AddActorLocalRotation // (Final|Native|Public|HasOutParms|HasDefaults|BlueprintCallable) // @ game+0x368a6a0
	void K2_AddActorLocalOffset(struct FVector DeltaLocation, bool bSweep, struct FHitResult& SweepHitResult, bool bTeleport); // Function Engine.Actor.K2_AddActorLocalOffset // (Final|Native|Public|HasOutParms|HasDefaults|BlueprintCallable) // @ game+0x368a4f0
	bool IsOverlappingActor(struct AActor* Other); // Function Engine.Actor.IsOverlappingActor // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x368a450
	bool IsChildActor(); // Function Engine.Actor.IsChildActor // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x368a420
	bool IsActorTickEnabled(); // Function Engine.Actor.IsActorTickEnabled // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x368a3f0
	bool IsActorBeingDestroyed(); // Function Engine.Actor.IsActorBeingDestroyed // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x368a3c0
	bool HasAuthority(); // Function Engine.Actor.HasAuthority // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x368a390
	float GetVerticalDistanceTo(struct AActor* OtherActor); // Function Engine.Actor.GetVerticalDistanceTo // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x368a2f0
	struct FVector GetVelocity(); // Function Engine.Actor.GetVelocity // (Native|Public|HasDefaults|BlueprintCallable|BlueprintPure|Const) // @ game+0x368a2b0
	struct FTransform GetTransform(); // Function Engine.Actor.GetTransform // (Final|Native|Public|HasDefaults|BlueprintCallable|BlueprintPure|Const) // @ game+0x368a260
	bool GetTickableWhenPaused(); // Function Engine.Actor.GetTickableWhenPaused // (Final|Native|Public|BlueprintCallable) // @ game+0x368a230
	float GetSquaredHorizontalDistanceTo(struct AActor* OtherActor); // Function Engine.Actor.GetSquaredHorizontalDistanceTo // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x368a190
	float GetSquaredDistanceTo(struct AActor* OtherActor); // Function Engine.Actor.GetSquaredDistanceTo // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x368a0f0
	enum class ENetRole GetRemoteRole(); // Function Engine.Actor.GetRemoteRole // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x368a0d0
	struct UChildActorComponent* GetParentComponent(); // Function Engine.Actor.GetParentComponent // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x368a0a0
	struct AActor* GetParentActor(); // Function Engine.Actor.GetParentActor // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x368a070
	struct AActor* GetOwner(); // Function Engine.Actor.GetOwner // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x368a050
	void GetOverlappingComponents(struct TArray<struct UPrimitiveComponent*>& OverlappingComponents); // Function Engine.Actor.GetOverlappingComponents // (Final|Native|Public|HasOutParms|BlueprintCallable|BlueprintPure|Const) // @ game+0x3689fa0
	void GetOverlappingActors(struct TArray<struct AActor*>& OverlappingActors, struct AActor* ClassFilter); // Function Engine.Actor.GetOverlappingActors // (Final|Native|Public|HasOutParms|BlueprintCallable|BlueprintPure|Const) // @ game+0x3689ea0
	enum class ENetRole GetLocalRole(); // Function Engine.Actor.GetLocalRole // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x3689e80
	float GetLifeSpan(); // Function Engine.Actor.GetLifeSpan // (Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x3689e40
	struct AController* GetInstigatorController(); // Function Engine.Actor.GetInstigatorController // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x3689e10
	struct APawn* GetInstigator(); // Function Engine.Actor.GetInstigator // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x3689de0
	struct FVector GetInputVectorAxisValue(struct FKey InputAxisKey); // Function Engine.Actor.GetInputVectorAxisValue // (Final|Native|Public|HasDefaults|BlueprintCallable|BlueprintPure|Const) // @ game+0x3689cd0
	float GetInputAxisValue(struct FName InputAxisName); // Function Engine.Actor.GetInputAxisValue // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x3689c30
	float GetInputAxisKeyValue(struct FKey InputAxisKey); // Function Engine.Actor.GetInputAxisKeyValue // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x3689b30
	float GetHorizontalDotProductTo(struct AActor* OtherActor); // Function Engine.Actor.GetHorizontalDotProductTo // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x3689a90
	float GetHorizontalDistanceTo(struct AActor* OtherActor); // Function Engine.Actor.GetHorizontalDistanceTo // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x36899f0
	float GetGameTimeSinceCreation(); // Function Engine.Actor.GetGameTimeSinceCreation // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x36899c0
	float GetDotProductTo(struct AActor* OtherActor); // Function Engine.Actor.GetDotProductTo // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x3689920
	float GetDistanceTo(struct AActor* OtherActor); // Function Engine.Actor.GetDistanceTo // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x3689880
	struct TArray<struct UActorComponent*> GetComponentsByTag(struct UActorComponent* ComponentClass, struct FName Tag); // Function Engine.Actor.GetComponentsByTag // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x3689770
	struct TArray<struct UActorComponent*> GetComponentsByInterface(struct UInterface* Interface); // Function Engine.Actor.GetComponentsByInterface // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x3689690
	struct UActorComponent* GetComponentByClass(struct UActorComponent* ComponentClass); // Function Engine.Actor.GetComponentByClass // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x36895f0
	struct FName GetAttachParentSocketName(); // Function Engine.Actor.GetAttachParentSocketName // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x36894b0
	struct AActor* GetAttachParentActor(); // Function Engine.Actor.GetAttachParentActor // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x3689480
	void GetAttachedActors(struct TArray<struct AActor*>& OutActors, bool bResetArray); // Function Engine.Actor.GetAttachedActors // (Final|Native|Public|HasOutParms|BlueprintCallable|BlueprintPure|Const) // @ game+0x36894f0
	void GetAllChildActors(struct TArray<struct AActor*>& ChildActors, bool bIncludeDescendants); // Function Engine.Actor.GetAllChildActors // (Final|Native|Public|HasOutParms|BlueprintCallable|BlueprintPure|Const) // @ game+0x3689380
	struct FVector GetActorUpVector(); // Function Engine.Actor.GetActorUpVector // (Final|Native|Public|HasDefaults|BlueprintCallable|BlueprintPure|Const) // @ game+0x3689310
	float GetActorTimeDilation(); // Function Engine.Actor.GetActorTimeDilation // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x36892e0
	float GetActorTickInterval(); // Function Engine.Actor.GetActorTickInterval // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x36892b0
	struct FVector GetActorScale3D(); // Function Engine.Actor.GetActorScale3D // (Final|Native|Public|HasDefaults|BlueprintCallable|BlueprintPure|Const) // @ game+0x3689270
	struct FVector GetActorRightVector(); // Function Engine.Actor.GetActorRightVector // (Final|Native|Public|HasDefaults|BlueprintCallable|BlueprintPure|Const) // @ game+0x3689200
	struct FVector GetActorRelativeScale3D(); // Function Engine.Actor.GetActorRelativeScale3D // (Final|Native|Public|HasDefaults|BlueprintCallable|BlueprintPure|Const) // @ game+0x36891c0
	struct FVector GetActorForwardVector(); // Function Engine.Actor.GetActorForwardVector // (Final|Native|Public|HasDefaults|BlueprintCallable|BlueprintPure|Const) // @ game+0x3689150
	void GetActorEyesViewPoint(struct FVector& OutLocation, struct FRotator& OutRotation); // Function Engine.Actor.GetActorEyesViewPoint // (Native|Public|HasOutParms|HasDefaults|BlueprintCallable|BlueprintPure|Const) // @ game+0x3689060
	bool GetActorEnableCollision(); // Function Engine.Actor.GetActorEnableCollision // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x3689030
	void GetActorBounds(bool bOnlyCollidingComponents, struct FVector& Origin, struct FVector& BoxExtent, bool bIncludeFromChildActors); // Function Engine.Actor.GetActorBounds // (Final|Native|Public|HasOutParms|HasDefaults|BlueprintCallable|BlueprintPure|Const) // @ game+0x3688ea0
	void ForceNetUpdate(); // Function Engine.Actor.ForceNetUpdate // (Native|Public|BlueprintCallable) // @ game+0x1664bc0
	void FlushNetDormancy(); // Function Engine.Actor.FlushNetDormancy // (Final|BlueprintAuthorityOnly|Native|Public|BlueprintCallable) // @ game+0x3688e80
	void FinishAddComponent(struct UActorComponent* Component, bool bManualAttachment, struct FTransform& RelativeTransform); // Function Engine.Actor.FinishAddComponent // (Final|Native|Public|HasOutParms|HasDefaults|BlueprintCallable) // @ game+0x3688d20
	void EnableInput(struct APlayerController* PlayerController); // Function Engine.Actor.EnableInput // (Native|Public|BlueprintCallable) // @ game+0x166fca0
	void DisableInput(struct APlayerController* PlayerController); // Function Engine.Actor.DisableInput // (Native|Public|BlueprintCallable) // @ game+0x3688c90
	void DetachRootComponentFromParent(bool bMaintainWorldPosition); // Function Engine.Actor.DetachRootComponentFromParent // (Final|Native|Public|BlueprintCallable) // @ game+0x3688c00
	void AddTickPrerequisiteComponent(struct UActorComponent* PrerequisiteComponent); // Function Engine.Actor.AddTickPrerequisiteComponent // (Native|Public|BlueprintCallable) // @ game+0x3688b70
	void AddTickPrerequisiteActor(struct AActor* PrerequisiteActor); // Function Engine.Actor.AddTickPrerequisiteActor // (Native|Public|BlueprintCallable) // @ game+0x3688ae0
	struct UActorComponent* AddComponentByClass(struct UActorComponent* Class, bool bManualAttachment, struct FTransform& RelativeTransform, bool bDeferredFinish); // Function Engine.Actor.AddComponentByClass // (Final|Native|Public|HasOutParms|HasDefaults|BlueprintCallable) // @ game+0x3688920
	struct UActorComponent* AddComponent(struct FName TemplateName, bool bManualAttachment, struct FTransform& RelativeTransform, struct UObject* ComponentTemplateContext, bool bDeferredFinish); // Function Engine.Actor.AddComponent // (Final|Native|Public|HasOutParms|HasDefaults|BlueprintCallable) // @ game+0x3688710
	bool ActorHasTag(struct FName Tag); // Function Engine.Actor.ActorHasTag // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x3688670

};

// Class Engine.Controller 
struct AController : AActor {
	char pad_220[0x8]; // 0x220(0x08)
	struct APlayerState* PlayerState; // 0x228(0x08)
	char pad_230[0x8]; // 0x230(0x08)
	char OnInstigatedAnyDamage[0x10]; // 0x238(0x10)
	struct FName StateName; // 0x248(0x08)
	struct APawn* Pawn; // 0x250(0x08)
	char pad_258[0x8]; // 0x258(0x08)
	struct ACharacter* Character; // 0x260(0x08)
	struct USceneComponent* TransformComponent; // 0x268(0x08)
	char pad_270[0x18]; // 0x270(0x18)
	struct FRotator ControlRotation; // 0x288(0x0c)
	char bAttachToPawn : 1; // 0x294(0x01)
	char pad_294_1 : 7; // 0x294(0x01)
	char pad_295[0x3]; // 0x295(0x03)

	bool LineOfSightTo(AActor* Other);
	void SetControlRotation(FRotator NewRotation);
};

// Class Engine.Pawn
// Size: 0x280 (Inherited: 0x220)
struct APawn : AActor {
	char pad_220[0x8]; // 0x220(0x08)
	char bUseControllerRotationPitch : 1; // 0x228(0x01)
	char bUseControllerRotationYaw : 1; // 0x228(0x01)
	char bUseControllerRotationRoll : 1; // 0x228(0x01)
	char bCanAffectNavigationGeneration : 1; // 0x228(0x01)
	char pad_228_4 : 4; // 0x228(0x01)
	char pad_229[0x3]; // 0x229(0x03)
	float BaseEyeHeight; // 0x22c(0x04)
	char AutoPossessPlayer[0x01]; // 0x230(0x01)
	char AutoPossessAI[0x01]; // 0x231(0x01)
	char RemoteViewPitch[0x01]; // 0x232(0x01)
	char pad_233[0x5]; // 0x233(0x05)
	struct AController* AIControllerClass; // 0x238(0x08)
	struct APlayerState* PlayerState; // 0x240(0x08)
	char pad_248[0x8]; // 0x248(0x08)
	struct AController* LastHitBy; // 0x250(0x08)
	struct AController* Controller; // 0x258(0x08)
	char pad_260[0x4]; // 0x260(0x04)
	struct FVector ControlInputVector; // 0x264(0x0c)
	struct FVector LastControlInputVector; // 0x270(0x0c)
	char pad_27C[0x4]; // 0x27c(0x04)

	void SpawnDefaultController(); // Function Engine.Pawn.SpawnDefaultController // (Native|Public|BlueprintCallable) // @ game+0x1695cd0
	void SetCanAffectNavigationGeneration(bool bNewValue, bool bForceUpdate); // Function Engine.Pawn.SetCanAffectNavigationGeneration // (Final|Native|Public|BlueprintCallable) // @ game+0x37a6280
	void ReceiveUnpossessed(struct AController* OldController); // Function Engine.Pawn.ReceiveUnpossessed // (Event|Public|BlueprintEvent) // @ game+0x1a5c6b0
	void ReceivePossessed(struct AController* NewController); // Function Engine.Pawn.ReceivePossessed // (Event|Public|BlueprintEvent) // @ game+0x1a5c6b0
	void PawnMakeNoise(float Loudness, struct FVector NoiseLocation, bool bUseNoiseMakerLocation, struct AActor* NoiseMaker); // Function Engine.Pawn.PawnMakeNoise // (Final|BlueprintAuthorityOnly|Native|Public|HasDefaults|BlueprintCallable) // @ game+0x37a5720
	void OnRep_PlayerState(); // Function Engine.Pawn.OnRep_PlayerState // (Native|Public) // @ game+0x8f3b20
	void OnRep_Controller(); // Function Engine.Pawn.OnRep_Controller // (Native|Public) // @ game+0x16be520
	void LaunchPawn(struct FVector LaunchVelocity, bool bXYOverride, bool bZOverride); // Function Engine.Pawn.LaunchPawn // (Final|Native|Public|HasDefaults|BlueprintCallable) // @ game+0x37a54e0
	struct FVector K2_GetMovementInputVector(); // Function Engine.Pawn.K2_GetMovementInputVector // (Final|Native|Public|HasDefaults|BlueprintCallable|BlueprintPure|Const) // @ game+0x37a5380
	bool IsPlayerControlled(); // Function Engine.Pawn.IsPlayerControlled // (Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x36d0ba0
	bool IsPawnControlled(); // Function Engine.Pawn.IsPawnControlled // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x37a54b0
	bool IsMoveInputIgnored(); // Function Engine.Pawn.IsMoveInputIgnored // (Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x37a5450
	bool IsLocallyControlled(); // Function Engine.Pawn.IsLocallyControlled // (Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x37a5420
	bool IsControlled(); // Function Engine.Pawn.IsControlled // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x37a53f0
	bool IsBotControlled(); // Function Engine.Pawn.IsBotControlled // (Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x36f3aa0
	struct FVector GetPendingMovementInputVector(); // Function Engine.Pawn.GetPendingMovementInputVector // (Final|Native|Public|HasDefaults|BlueprintCallable|BlueprintPure|Const) // @ game+0x37a5380
	struct FVector GetNavAgentLocation(); // Function Engine.Pawn.GetNavAgentLocation // (Native|Public|HasDefaults|BlueprintCallable|BlueprintPure|Const) // @ game+0x37a52c0
	struct UPawnMovementComponent* GetMovementComponent(); // Function Engine.Pawn.GetMovementComponent // (Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x36d96c0
	struct AActor* GetMovementBaseActor(struct APawn* Pawn); // Function Engine.Pawn.GetMovementBaseActor // (Final|Native|Static|Public|BlueprintCallable|BlueprintPure) // @ game+0x37a5230
	struct FVector GetLastMovementInputVector(); // Function Engine.Pawn.GetLastMovementInputVector // (Final|Native|Public|HasDefaults|BlueprintCallable|BlueprintPure|Const) // @ game+0x37a51f0
	struct FRotator GetControlRotation(); // Function Engine.Pawn.GetControlRotation // (Final|Native|Public|HasDefaults|BlueprintCallable|BlueprintPure|Const) // @ game+0x37a50c0
	struct AController* GetController(); // Function Engine.Pawn.GetController // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x37a5100
	struct FRotator GetBaseAimRotation(); // Function Engine.Pawn.GetBaseAimRotation // (Native|Public|HasDefaults|BlueprintCallable|BlueprintPure|Const) // @ game+0x37a4ee0
	void DetachFromControllerPendingDestroy(); // Function Engine.Pawn.DetachFromControllerPendingDestroy // (Native|Public|BlueprintCallable) // @ game+0x16964e0
	struct FVector ConsumeMovementInputVector(); // Function Engine.Pawn.ConsumeMovementInputVector // (Native|Public|HasDefaults|BlueprintCallable) // @ game+0x37a4ea0
	void AddMovementInput(struct FVector WorldDirection, float ScaleValue, bool bForce); // Function Engine.Pawn.AddMovementInput // (Native|Public|HasDefaults|BlueprintCallable) // @ game+0x37a4980
	void AddControllerYawInput(float Val); // Function Engine.Pawn.AddControllerYawInput // (Native|Public|BlueprintCallable) // @ game+0x37a4810
	void AddControllerRollInput(float Val); // Function Engine.Pawn.AddControllerRollInput // (Native|Public|BlueprintCallable) // @ game+0x37a4780
	void AddControllerPitchInput(float Val); // Function Engine.Pawn.AddControllerPitchInput // (Native|Public|BlueprintCallable) // @ game+0x37a46f0
};

// Class Engine.Character
// Size: 0x4c0 (Inherited: 0x280)
struct ACharacter : APawn {
	struct USkeletalMeshComponent* Mesh; // 0x280(0x08)
	struct UCharacterMovementComponent* CharacterMovement; // 0x288(0x08)
	struct UCapsuleComponent* CapsuleComponent; // 0x290(0x08)
	char BasedMovement[0x30]; // 0x298(0x30)
	char ReplicatedBasedMovement[0x30]; // 0x2c8(0x30)
	float AnimRootMotionTranslationScale; // 0x2f8(0x04)
	struct FVector BaseTranslationOffset; // 0x2fc(0x0c)
	char pad_308[0x8]; // 0x308(0x08)
	char BaseRotationOffset[0x10]; // 0x310(0x10)
	float ReplicatedServerLastTransformUpdateTimeStamp; // 0x320(0x04)
	float ReplayLastTransformUpdateTimeStamp; // 0x324(0x04)
	char ReplicatedMovementMode; // 0x328(0x01)
	bool bInBaseReplication; // 0x329(0x01)
	char pad_32A[0x2]; // 0x32a(0x02)
	float CrouchedEyeHeight; // 0x32c(0x04)
	char bIsCrouched : 1; // 0x330(0x01)
	char bProxyIsJumpForceApplied : 1; // 0x330(0x01)
	char bPressedJump : 1; // 0x330(0x01)
	char bClientUpdating : 1; // 0x330(0x01)
	char bClientWasFalling : 1; // 0x330(0x01)
	char bClientResimulateRootMotion : 1; // 0x330(0x01)
	char bClientResimulateRootMotionSources : 1; // 0x330(0x01)
	char bSimGravityDisabled : 1; // 0x330(0x01)
	char bClientCheckEncroachmentOnNetUpdate : 1; // 0x331(0x01)
	char bServerMoveIgnoreRootMotion : 1; // 0x331(0x01)
	char bWasJumping : 1; // 0x331(0x01)
	char pad_331_3 : 5; // 0x331(0x01)
	char pad_332[0x2]; // 0x332(0x02)
	float JumpKeyHoldTime; // 0x334(0x04)
	float JumpForceTimeRemaining; // 0x338(0x04)
	float ProxyJumpForceStartedTime; // 0x33c(0x04)
	float JumpMaxHoldTime; // 0x340(0x04)
	int32_t JumpMaxCount; // 0x344(0x04)
	int32_t JumpCurrentCount; // 0x348(0x04)
	int32_t JumpCurrentCountPreJump; // 0x34c(0x04)
	char pad_350[0x8]; // 0x350(0x08)
	struct FMulticastInlineDelegate OnReachedJumpApex; // 0x358(0x10)
	char pad_368[0x10]; // 0x368(0x10)
	struct FMulticastInlineDelegate MovementModeChangedDelegate; // 0x378(0x10)
	struct FMulticastInlineDelegate OnCharacterMovementUpdated; // 0x388(0x10)
	char SavedRootMotion[0x38]; // 0x398(0x38)
	char ClientRootMotionParams[0x40]; // 0x3d0(0x40)
	struct TArray<struct FSimulatedRootMotionReplicatedMove> RootMotionRepMoves; // 0x410(0x10)
	char RepRootMotion[0x98]; // 0x420(0x98)
	char pad_4B8[0x8]; // 0x4b8(0x08)

	void UnCrouch(bool bClientSimulation); // Function Engine.Character.UnCrouch // (Native|Public|BlueprintCallable) // @ game+0x36c86d0
	void StopJumping(); // Function Engine.Character.StopJumping // (Native|Public|BlueprintCallable) // @ game+0x1696150
	void StopAnimMontage(struct UAnimMontage* AnimMontage); // Function Engine.Character.StopAnimMontage // (Native|Public|BlueprintCallable) // @ game+0x36c8640
	void ServerMovePacked(struct FCharacterServerMovePackedBits PackedBits); // Function Engine.Character.ServerMovePacked // (Net|Native|Event|Public|NetServer|NetValidate) // @ game+0x36c8270
	void ServerMoveOld(float OldTimeStamp, struct FVector_NetQuantize10 OldAccel, char OldMoveFlags); // Function Engine.Character.ServerMoveOld // (Net|Native|Event|Public|NetServer|NetValidate) // @ game+0x36c8100
	void ServerMoveNoBase(float Timestamp, struct FVector_NetQuantize10 InAccel, struct FVector_NetQuantize100 ClientLoc, char CompressedMoveFlags, char ClientRoll, uint32_t View, char ClientMovementMode); // Function Engine.Character.ServerMoveNoBase // (Net|Native|Event|Public|NetServer|NetValidate) // @ game+0x36c7e60
	void ServerMoveDualNoBase(float TimeStamp0, struct FVector_NetQuantize10 InAccel0, char PendingFlags, uint32_t View0, float Timestamp, struct FVector_NetQuantize10 InAccel, struct FVector_NetQuantize100 ClientLoc, char NewFlags, char ClientRoll, uint32_t View, char ClientMovementMode); // Function Engine.Character.ServerMoveDualNoBase // (Net|Native|Event|Public|NetServer|NetValidate) // @ game+0x36c7a60
	void ServerMoveDualHybridRootMotion(float TimeStamp0, struct FVector_NetQuantize10 InAccel0, char PendingFlags, uint32_t View0, float Timestamp, struct FVector_NetQuantize10 InAccel, struct FVector_NetQuantize100 ClientLoc, char NewFlags, char ClientRoll, uint32_t View, struct UPrimitiveComponent* ClientMovementBase, struct FName ClientBaseBoneName, char ClientMovementMode); // Function Engine.Character.ServerMoveDualHybridRootMotion // (Net|Native|Event|Public|NetServer|NetValidate) // @ game+0x36c75d0
	void ServerMoveDual(float TimeStamp0, struct FVector_NetQuantize10 InAccel0, char PendingFlags, uint32_t View0, float Timestamp, struct FVector_NetQuantize10 InAccel, struct FVector_NetQuantize100 ClientLoc, char NewFlags, char ClientRoll, uint32_t View, struct UPrimitiveComponent* ClientMovementBase, struct FName ClientBaseBoneName, char ClientMovementMode); // Function Engine.Character.ServerMoveDual // (Net|Native|Event|Public|NetServer|NetValidate) // @ game+0x36c7140
	void ServerMove(float Timestamp, struct FVector_NetQuantize10 InAccel, struct FVector_NetQuantize100 ClientLoc, char CompressedMoveFlags, char ClientRoll, uint32_t View, struct UPrimitiveComponent* ClientMovementBase, struct FName ClientBaseBoneName, char ClientMovementMode); // Function Engine.Character.ServerMove // (Net|Native|Event|Public|NetServer|NetValidate) // @ game+0x36c6e00
	void RootMotionDebugClientPrintOnScreen(struct FString inString); // Function Engine.Character.RootMotionDebugClientPrintOnScreen // (Net|NetReliableNative|Event|Public|NetClient) // @ game+0x36c6d60
	float PlayAnimMontage(struct UAnimMontage* AnimMontage, float InPlayRate, struct FName StartSectionName); // Function Engine.Character.PlayAnimMontage // (Native|Public|BlueprintCallable) // @ game+0x36c6c40
	void OnWalkingOffLedge(struct FVector& PreviousFloorImpactNormal, struct FVector& PreviousFloorContactNormal, struct FVector& PreviousLocation, float TimeDelta); // Function Engine.Character.OnWalkingOffLedge // (Native|Event|Public|HasOutParms|HasDefaults|BlueprintEvent) // @ game+0x36c6aa0
	void OnRep_RootMotion(); // Function Engine.Character.OnRep_RootMotion // (Final|Native|Public) // @ game+0x36c6a80
	void OnRep_ReplicatedBasedMovement(); // Function Engine.Character.OnRep_ReplicatedBasedMovement // (Native|Public) // @ game+0x16a2e50
	void OnRep_ReplayLastTransformUpdateTimeStamp(); // Function Engine.Character.OnRep_ReplayLastTransformUpdateTimeStamp // (Final|Native|Public) // @ game+0x36c6a60
	void OnRep_IsCrouched(); // Function Engine.Character.OnRep_IsCrouched // (Native|Public) // @ game+0x16a3ab0
	void OnLaunched(struct FVector LaunchVelocity, bool bXYOverride, bool bZOverride); // Function Engine.Character.OnLaunched // (Event|Public|HasDefaults|BlueprintEvent) // @ game+0x1a5c6b0
	void OnLanded(struct FHitResult& Hit); // Function Engine.Character.OnLanded // (Event|Public|HasOutParms|BlueprintEvent) // @ game+0x1a5c6b0
	void OnJumped(); // Function Engine.Character.OnJumped // (Native|Event|Public|BlueprintEvent) // @ game+0x36c6a40
	void LaunchCharacter(struct FVector LaunchVelocity, bool bXYOverride, bool bZOverride); // Function Engine.Character.LaunchCharacter // (Native|Public|HasDefaults|BlueprintCallable) // @ game+0x36c6910
	void K2_UpdateCustomMovement(float DeltaTime); // Function Engine.Character.K2_UpdateCustomMovement // (Event|Public|BlueprintEvent) // @ game+0x1a5c6b0
	void K2_OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust); // Function Engine.Character.K2_OnStartCrouch // (Event|Public|BlueprintEvent) // @ game+0x1a5c6b0
	void K2_OnMovementModeChanged(enum class EMovementMode PrevMovementMode, enum class EMovementMode NewMovementMode, char PrevCustomMode, char NewCustomMode); // Function Engine.Character.K2_OnMovementModeChanged // (Event|Public|BlueprintEvent) // @ game+0x1a5c6b0
	void K2_OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust); // Function Engine.Character.K2_OnEndCrouch // (Event|Public|BlueprintEvent) // @ game+0x1a5c6b0
	void Jump(); // Function Engine.Character.Jump // (Native|Public|BlueprintCallable) // @ game+0x36c68f0
	bool IsPlayingRootMotion(); // Function Engine.Character.IsPlayingRootMotion // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x36c68c0
	bool IsPlayingNetworkedRootMotionMontage(); // Function Engine.Character.IsPlayingNetworkedRootMotionMontage // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x36c6890
	bool IsJumpProvidingForce(); // Function Engine.Character.IsJumpProvidingForce // (Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x36c6860
	bool HasAnyRootMotion(); // Function Engine.Character.HasAnyRootMotion // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x36c6830
	struct UAnimMontage* GetCurrentMontage(); // Function Engine.Character.GetCurrentMontage // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x36c61a0
	struct FVector GetBaseTranslationOffset(); // Function Engine.Character.GetBaseTranslationOffset // (Final|Native|Public|HasDefaults|BlueprintCallable|BlueprintPure|Const) // @ game+0x36c6170
	struct FRotator GetBaseRotationOffsetRotator(); // Function Engine.Character.GetBaseRotationOffsetRotator // (Final|Native|Public|HasDefaults|BlueprintCallable|BlueprintPure|Const) // @ game+0x36c6120
	float GetAnimRootMotionTranslationScale(); // Function Engine.Character.GetAnimRootMotionTranslationScale // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x36c60f0
	void Crouch(bool bClientSimulation); // Function Engine.Character.Crouch // (Native|Public|BlueprintCallable) // @ game+0x36c6060
	void ClientVeryShortAdjustPosition(float Timestamp, struct FVector NewLoc, struct UPrimitiveComponent* NewBase, struct FName NewBaseBoneName, bool bHasBase, bool bBaseRelativePosition, char ServerMovementMode); // Function Engine.Character.ClientVeryShortAdjustPosition // (Net|Native|Event|Public|HasDefaults|NetClient) // @ game+0x36c5ce0
	void ClientMoveResponsePacked(struct FCharacterMoveResponsePackedBits PackedBits); // Function Engine.Character.ClientMoveResponsePacked // (Net|Native|Event|Public|NetClient|NetValidate) // @ game+0x36c5bf0
	void ClientCheatWalk(); // Function Engine.Character.ClientCheatWalk // (Net|NetReliableNative|Event|Public|NetClient) // @ game+0x36c5bd0
	void ClientCheatGhost(); // Function Engine.Character.ClientCheatGhost // (Net|NetReliableNative|Event|Public|NetClient) // @ game+0x36c5bb0
	void ClientCheatFly(); // Function Engine.Character.ClientCheatFly // (Net|NetReliableNative|Event|Public|NetClient) // @ game+0x36c5b90
	void ClientAdjustRootMotionSourcePosition(float Timestamp, struct FRootMotionSourceGroup ServerRootMotion, bool bHasAnimRootMotion, float ServerMontageTrackPosition, struct FVector ServerLoc, struct FVector_NetQuantizeNormal ServerRotation, float ServerVelZ, struct UPrimitiveComponent* ServerBase, struct FName ServerBoneName, bool bHasBase, bool bBaseRelativePosition, char ServerMovementMode); // Function Engine.Character.ClientAdjustRootMotionSourcePosition // (Net|Native|Event|Public|HasDefaults|NetClient) // @ game+0x36c56b0
	void ClientAdjustRootMotionPosition(float Timestamp, float ServerMontageTrackPosition, struct FVector ServerLoc, struct FVector_NetQuantizeNormal ServerRotation, float ServerVelZ, struct UPrimitiveComponent* ServerBase, struct FName ServerBoneName, bool bHasBase, bool bBaseRelativePosition, char ServerMovementMode); // Function Engine.Character.ClientAdjustRootMotionPosition // (Net|Native|Event|Public|HasDefaults|NetClient) // @ game+0x36c5380
	void ClientAdjustPosition(float Timestamp, struct FVector NewLoc, struct FVector NewVel, struct UPrimitiveComponent* NewBase, struct FName NewBaseBoneName, bool bHasBase, bool bBaseRelativePosition, char ServerMovementMode); // Function Engine.Character.ClientAdjustPosition // (Net|Native|Event|Public|HasDefaults|NetClient) // @ game+0x36c50f0
	void ClientAckGoodMove(float Timestamp); // Function Engine.Character.ClientAckGoodMove // (Net|Native|Event|Public|NetClient) // @ game+0x36c5070
	bool CanJumpInternal(); // Function Engine.Character.CanJumpInternal // (Native|Event|Protected|BlueprintEvent|Const) // @ game+0x36c5040
	bool CanJump(); // Function Engine.Character.CanJump // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x36c5010
	bool CanCrouch(); // Function Engine.Character.CanCrouch // (Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x36c4fe0
	void CacheInitialMeshOffset(struct FVector MeshRelativeLocation, struct FRotator MeshRelativeRotation); // Function Engine.Character.CacheInitialMeshOffset // (Native|Public|HasDefaults|BlueprintCallable) // @ game+0x36c4ef0
};

// Class PortalWars.PortalWarsCharacter
// Size: 0xde0 (Inherited: 0x4c0)
struct APortalWarsCharacter : ACharacter {
	char pad_4C0[0x18]; // 0x4c0(0x18)
	struct APortalWarsGameState* GameStateRef; // 0x4d8(0x08)
	char pad_4E0[0xc]; // 0x4e0(0x0c)
	float Health; // 0x4ec(0x04)
	float MaxHealth; // 0x4f0(0x04)
	char pad_4F4[0x8]; // 0x4f4(0x08)
	float healthRechargeDelay; // 0x4fc(0x04)
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

	void UpdateSkins(); // Function PortalWars.PortalWarsCharacter.UpdateSkins // (Final|Native|Protected) // @ game+0x166a4a0
	void updateCollisionProfiles(); // Function PortalWars.PortalWarsCharacter.updateCollisionProfiles // (Final|Native|Protected) // @ game+0x166a7b0
	void StopEmoteInMenu(); // Function PortalWars.PortalWarsCharacter.StopEmoteInMenu // (Final|Native|Protected) // @ game+0x166a480
	void StopEmote(); // Function PortalWars.PortalWarsCharacter.StopEmote // (Final|Native|Protected) // @ game+0x166a460
	void SpawnSpray_Multicast(struct FVector_NetQuantize Location, struct FVector_NetQuantizeNormal Rotation); // Function PortalWars.PortalWarsCharacter.SpawnSpray_Multicast // (Net|Native|Event|NetMulticast|Protected) // @ game+0x166a370
	void ServerTeleport(struct FTransform_NetQuantize CharacterTransform_world, struct FVector_NetQuantize NewVel, struct FQuat NewControlRotation, struct APortal* PortalUsed); // Function PortalWars.PortalWarsCharacter.ServerTeleport // (Net|NetReliableNative|Event|Public|NetServer|HasDefaults|NetValidate) // @ game+0x166a180
	void ServerStartMelee(struct APortalWarsCharacter* InEnemyToMelee, struct FVector_NetQuantize InVelocity); // Function PortalWars.PortalWarsCharacter.ServerStartMelee // (Net|NetReliableNative|Event|Protected|NetServer|NetValidate) // @ game+0x166a080
	void ServerSpawnSpray(struct FVector_NetQuantize Location, struct FVector_NetQuantizeNormal Rotation); // Function PortalWars.PortalWarsCharacter.ServerSpawnSpray // (Net|Native|Event|Protected|NetServer|NetValidate) // @ game+0x1669f40
	void ServerSetZooming(bool bNewZooming); // Function PortalWars.PortalWarsCharacter.ServerSetZooming // (Net|NetReliableNative|Event|Protected|NetServer|NetValidate) // @ game+0x1669e80
	void ServerSetOverlappingPortalCollisionProfiles(); // Function PortalWars.PortalWarsCharacter.ServerSetOverlappingPortalCollisionProfiles // (Net|NetReliableNative|Event|Public|NetServer|NetValidate) // @ game+0x1669e30
	void ServerSetDefaultCollisionProfiles(); // Function PortalWars.PortalWarsCharacter.ServerSetDefaultCollisionProfiles // (Net|NetReliableNative|Event|Public|NetServer|NetValidate) // @ game+0x1669de0
	void ServerRequestSuicide(); // Function PortalWars.PortalWarsCharacter.ServerRequestSuicide // (Net|Native|Event|Public|NetServer|NetValidate) // @ game+0x1669d90
	void ServerPlayEmote(); // Function PortalWars.PortalWarsCharacter.ServerPlayEmote // (Net|Native|Event|Protected|NetServer|NetValidate) // @ game+0x1669d40
	void ServerInterruptEmote(); // Function PortalWars.PortalWarsCharacter.ServerInterruptEmote // (Net|Native|Event|Protected|NetServer|NetValidate) // @ game+0x1669cf0
	void ServerHandleTeabagging(struct TArray<struct APortalWarsCharacter*> deadBodies); // Function PortalWars.PortalWarsCharacter.ServerHandleTeabagging // (Net|NetReliableNative|Event|Public|NetServer|NetValidate) // @ game+0x1669c30
	void ServerFinishMelee(struct FVector_NetQuantize100 EndingLocation); // Function PortalWars.PortalWarsCharacter.ServerFinishMelee // (Net|NetReliableNative|Event|Protected|NetServer|NetValidate) // @ game+0x1669b60
	void ServerEquipWeapon(struct AGun* NewWeapon); // Function PortalWars.PortalWarsCharacter.ServerEquipWeapon // (Net|NetReliableNative|Event|Protected|NetServer|NetValidate) // @ game+0x1669aa0
	void ServerAttemptManualPickup(struct APortalWarsGunPickup* GunPickup, struct FVector_NetQuantize CurrentWeaponLocation, struct FVector_NetQuantizeNormal CurrentWeaponRotation); // Function PortalWars.PortalWarsCharacter.ServerAttemptManualPickup // (Net|NetReliableNative|Event|Public|NetServer|NetValidate) // @ game+0x1669920
	void ServerApplyMeleeDamage(struct APortalWarsCharacter* EnemyToDamage); // Function PortalWars.PortalWarsCharacter.ServerApplyMeleeDamage // (Net|NetReliableNative|Event|Protected|NetServer|NetValidate) // @ game+0x1669860
	void RequestSuicide(); // Function PortalWars.PortalWarsCharacter.RequestSuicide // (Final|Native|Public|BlueprintCallable) // @ game+0x1669840
	void PlayEmoteInMenu_Internal(); // Function PortalWars.PortalWarsCharacter.PlayEmoteInMenu_Internal // (Final|Native|Protected) // @ game+0x16697a0
	void PlayEmote_Multicast(); // Function PortalWars.PortalWarsCharacter.PlayEmote_Multicast // (Net|Native|Event|NetMulticast|Protected) // @ game+0x16697c0
	void OnSpray(); // Function PortalWars.PortalWarsCharacter.OnSpray // (Native|Public) // @ game+0x16696f0
	void OnReplayCameraModeChanged(enum class EReplayCameraMode NewReplayCameraMode); // Function PortalWars.PortalWarsCharacter.OnReplayCameraModeChanged // (Final|Native|Public) // @ game+0x1669670
	void OnRep_PortalGun(); // Function PortalWars.PortalWarsCharacter.OnRep_PortalGun // (Final|Native|Protected) // @ game+0x1669650
	void OnRep_LastCausedHitInfo(); // Function PortalWars.PortalWarsCharacter.OnRep_LastCausedHitInfo // (Final|Native|Public) // @ game+0x1669630
	void OnRep_IsZooming(); // Function PortalWars.PortalWarsCharacter.OnRep_IsZooming // (Final|Native|Protected) // @ game+0x1669610
	void OnRep_IsThrusting(); // Function PortalWars.PortalWarsCharacter.OnRep_IsThrusting // (Native|Public) // @ game+0x16695f0
	void OnRep_IsSprinting(); // Function PortalWars.PortalWarsCharacter.OnRep_IsSprinting // (Final|Native|Protected) // @ game+0x16695d0
	void OnRep_IsClambering(); // Function PortalWars.PortalWarsCharacter.OnRep_IsClambering // (Final|Native|Protected) // @ game+0x16695b0
	void OnRep_Inventory(); // Function PortalWars.PortalWarsCharacter.OnRep_Inventory // (Final|Native|Protected) // @ game+0x1669590
	void OnRep_IgnoreServerCorrections(); // Function PortalWars.PortalWarsCharacter.OnRep_IgnoreServerCorrections // (Final|Native|Protected) // @ game+0x1669570
	void OnRep_Health(); // Function PortalWars.PortalWarsCharacter.OnRep_Health // (Final|Native|Public) // @ game+0x1669550
	void OnRep_GrenadeLauncher(); // Function PortalWars.PortalWarsCharacter.OnRep_GrenadeLauncher // (Final|Native|Protected) // @ game+0x1669530
	void OnRep_CurTimeOutOfBounds(); // Function PortalWars.PortalWarsCharacter.OnRep_CurTimeOutOfBounds // (Final|Native|Protected) // @ game+0x1669480
	void OnRep_CurrentWeapon(struct AGun* LastWeapon); // Function PortalWars.PortalWarsCharacter.OnRep_CurrentWeapon // (Final|Native|Protected) // @ game+0x16694a0
	void OnKillcamStarted(); // Function PortalWars.PortalWarsCharacter.OnKillcamStarted // (Final|Native|Public) // @ game+0x1669440
	void OnKillcamEnded(); // Function PortalWars.PortalWarsCharacter.OnKillcamEnded // (Final|Native|Public) // @ game+0x1669420
	void OnGlobalVisionChanged(); // Function PortalWars.PortalWarsCharacter.OnGlobalVisionChanged // (Final|Native|Public) // @ game+0x1669250
	void OnGameplaySettingsChanged(); // Function PortalWars.PortalWarsCharacter.OnGameplaySettingsChanged // (Final|Native|Protected) // @ game+0x1669230
	void OnGameConfigUpdated(); // Function PortalWars.PortalWarsCharacter.OnGameConfigUpdated // (Native|Protected) // @ game+0x1669210
	void OnDeath(); // Function PortalWars.PortalWarsCharacter.OnDeath // (Final|Native|Public) // @ game+0x16691f0
	void MoveAndPostToMoveableAk(struct UAkAudioEvent* AkEvent, struct FVector NewLocation); // Function PortalWars.PortalWarsCharacter.MoveAndPostToMoveableAk // (Final|Native|Public|HasDefaults|BlueprintCallable) // @ game+0x1668d80
	bool IsZooming(); // Function PortalWars.PortalWarsCharacter.IsZooming // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x1668d50
	bool IsThirdPerson(); // Function PortalWars.PortalWarsCharacter.IsThirdPerson // (Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x1668d20
	bool IsSprinting(); // Function PortalWars.PortalWarsCharacter.IsSprinting // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x1668cf0
	bool IsLocallyViewed(); // Function PortalWars.PortalWarsCharacter.IsLocallyViewed // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x1668cc0
	bool IsFirstPerson(); // Function PortalWars.PortalWarsCharacter.IsFirstPerson // (Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x1668c90
	bool IsFiring(); // Function PortalWars.PortalWarsCharacter.IsFiring // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x1668c60
	bool IsEnemyFor(struct APortalWarsCharacter* Character); // Function PortalWars.PortalWarsCharacter.IsEnemyFor // (Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x1668bc0
	bool IsDead(); // Function PortalWars.PortalWarsCharacter.IsDead // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x1668b90
	bool IsAlive(); // Function PortalWars.PortalWarsCharacter.IsAlive // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x1668b60
	void InterruptEmote_Multicast(); // Function PortalWars.PortalWarsCharacter.InterruptEmote_Multicast // (Net|Native|Event|NetMulticast|Protected) // @ game+0x1668b40
	struct AGun* GetWeapon(); // Function PortalWars.PortalWarsCharacter.GetWeapon // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x1668aa0
	char GetTeamNum(); // Function PortalWars.PortalWarsCharacter.GetTeamNum // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x1668900
	float GetSprintingSpeedModifier(); // Function PortalWars.PortalWarsCharacter.GetSprintingSpeedModifier // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x16688e0
	struct AGun* GetSecondaryWeapon(); // Function PortalWars.PortalWarsCharacter.GetSecondaryWeapon // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x16688b0
	struct USkeletalMeshComponent* GetMesh1P(); // Function PortalWars.PortalWarsCharacter.GetMesh1P // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x1668890
	char GetEnemyTeamNum(); // Function PortalWars.PortalWarsCharacter.GetEnemyTeamNum // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x1668860
	struct UPortalWarsAkComponent* GetAkComponent(struct FName AkName); // Function PortalWars.PortalWarsCharacter.GetAkComponent // (Final|Native|Public|BlueprintCallable) // @ game+0x1668620
	struct FRotator GetAimOffsets(); // Function PortalWars.PortalWarsCharacter.GetAimOffsets // (Final|Native|Public|HasDefaults|BlueprintCallable|BlueprintPure) // @ game+0x16685e0
	void DestroyPortals(); // Function PortalWars.PortalWarsCharacter.DestroyPortals // (Final|Native|Public) // @ game+0x16685c0
	void DelayedServerFinishMelee(); // Function PortalWars.PortalWarsCharacter.DelayedServerFinishMelee // (Final|Native|Protected) // @ game+0x16685a0
	void ClientNotifyAlive(struct AController* C); // Function PortalWars.PortalWarsCharacter.ClientNotifyAlive // (Net|NetReliableNative|Event|Public|NetClient) // @ game+0x1668510
	void ClientFailedPickupAttempt(); // Function PortalWars.PortalWarsCharacter.ClientFailedPickupAttempt // (Net|NetReliableNative|Event|Public|NetClient) // @ game+0x16684f0
	void ClientEquipWeapon(struct AGun* NewWeapon); // Function PortalWars.PortalWarsCharacter.ClientEquipWeapon // (Net|NetReliableNative|Event|Protected|NetClient) // @ game+0x1668460
	void ClientDropSpecialItem(struct ASpecialItem* SpecialItem); // Function PortalWars.PortalWarsCharacter.ClientDropSpecialItem // (Net|NetReliableNative|Event|Public|NetClient) // @ game+0x16683d0
	void ClientDebugPortalRip(); // Function PortalWars.PortalWarsCharacter.ClientDebugPortalRip // (Net|NetReliableNative|Event|Public|NetClient) // @ game+0x16683b0
	void CheatSuicide(); // Function PortalWars.PortalWarsCharacter.CheatSuicide // (Final|Exec|Native|Protected) // @ game+0xf085f0
	void CheatShootAll(); // Function PortalWars.PortalWarsCharacter.CheatShootAll // (Final|Exec|Native|Protected) // @ game+0xf085f0
	void CheatProjectiles(); // Function PortalWars.PortalWarsCharacter.CheatProjectiles // (Final|Exec|Native|Protected) // @ game+0xf085f0
	void CheatPortalNoClip(); // Function PortalWars.PortalWarsCharacter.CheatPortalNoClip // (Final|Exec|Native|Protected) // @ game+0xf085f0
	void CheatPickup(); // Function PortalWars.PortalWarsCharacter.CheatPickup // (Final|Exec|Native|Protected) // @ game+0xf085f0
	void CheatMelee(); // Function PortalWars.PortalWarsCharacter.CheatMelee // (Final|Exec|Native|Protected) // @ game+0xf085f0
	void CheatAutoPickup(); // Function PortalWars.PortalWarsCharacter.CheatAutoPickup // (Final|Exec|Native|Protected) // @ game+0xf085f0
	void applyShotgunPointImpulsesToCorpseMulticast(struct TArray<struct FPWPointDamageEvent> DamageEvents); // Function PortalWars.PortalWarsCharacter.applyShotgunPointImpulsesToCorpseMulticast // (Net|Native|Event|NetMulticast|Public) // @ game+0x166a6e0
	void applyRadialImpulseToCorpseMulticast(struct FPWRadialDamageEvent RadialDamageEvent); // Function PortalWars.PortalWarsCharacter.applyRadialImpulseToCorpseMulticast // (Net|Native|Event|NetMulticast|Public) // @ game+0x166a600
	void applyPointImpulseToCorpseMulticast(struct FPWPointDamageEvent PointDamageEvent); // Function PortalWars.PortalWarsCharacter.applyPointImpulseToCorpseMulticast // (Net|Native|Event|NetMulticast|Public) // @ game+0x166a500
};

// ScriptStruct PortalWarsGlobals.PlayerStatsInfo
// Size: 0x68 (Inherited: 0x00)
struct FPlayerStatsInfo {
	int32_t Kills; // 0x00(0x04)
	int32_t Assists; // 0x04(0x04)
	int32_t Deaths; // 0x08(0x04)
	int32_t Suicides; // 0x0c(0x04)
	int32_t HeadshotKills; // 0x10(0x04)
	int32_t Teabags; // 0x14(0x04)
	int32_t MeleeKills; // 0x18(0x04)
	int32_t PortalKills; // 0x1c(0x04)
	int32_t KillsThruPortal; // 0x20(0x04)
	int32_t DoubleKills; // 0x24(0x04)
	int32_t TripleKills; // 0x28(0x04)
	int32_t QuadKills; // 0x2c(0x04)
	int32_t QuintKills; // 0x30(0x04)
	int32_t SexKills; // 0x34(0x04)
	int32_t Killstreak1; // 0x38(0x04)
	int32_t Killstreak2; // 0x3c(0x04)
	int32_t Killstreak3; // 0x40(0x04)
	int32_t Killstreak4; // 0x44(0x04)
	int32_t Killstreak5; // 0x48(0x04)
	int32_t Killstreak6; // 0x4c(0x04)
	int32_t DamageDealt; // 0x50(0x04)
	int32_t EmoteCount; // 0x54(0x04)
	struct TArray<struct FWeaponStats> Weapons; // 0x58(0x10)
};

// ScriptStruct PortalWarsGlobals.PlayerStatsInfo_InDepth
// Size: 0xc8 (Inherited: 0x68)
struct FPlayerStatsInfo_InDepth : FPlayerStatsInfo {
	int32_t EnemyPortalsDestroyed; // 0x68(0x04)
	int32_t AllyPortalsEntered; // 0x6c(0x04)
	int32_t EnemyPortalsEntered; // 0x70(0x04)
	int32_t OwnPortalsEntered; // 0x74(0x04)
	int32_t PortalsSpawned; // 0x78(0x04)
	int32_t DistancePortaled; // 0x7c(0x04)
	int32_t HighestConsecutiveKills; // 0x80(0x04)
	int32_t HillsCaptured; // 0x84(0x04)
	int32_t HillsNeutralized; // 0x88(0x04)
	int32_t KillsOnHill; // 0x8c(0x04)
	int32_t EnemyKillsOnHill; // 0x90(0x04)
	int32_t FlagsPickedUp; // 0x94(0x04)
	int32_t FlagsReturned; // 0x98(0x04)
	int32_t FlagKills; // 0x9c(0x04)
	int32_t FlagCarrierKills; // 0xa0(0x04)
	int32_t OddballsPickedUp; // 0xa4(0x04)
	int32_t OddballKills; // 0xa8(0x04)
	int32_t OddballCarrierKills; // 0xac(0x04)
	int32_t VipKills; // 0xb0(0x04)
	int32_t KillsAsVIP; // 0xb4(0x04)
	int32_t TeabagsDenied; // 0xb8(0x04)
	int32_t FirstBloods; // 0xbc(0x04)
	int32_t RevengeKills; // 0xc0(0x04)
	int32_t KingSlayers; // 0xc4(0x04)
};

// ScriptStruct PortalWarsGlobals.EquippedCustomizations
// Size: 0x10 (Inherited: 0x00)
struct FEquippedCustomizations {
	struct TArray<struct FEquippedCustomization> EquippedCustomizations; // 0x00(0x10)
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

// Class PortalWars.PortalWarsPlayerState
// Size: 0xb50 (Inherited: 0x320)
struct APortalWarsPlayerState : APlayerState {
	char pad_320[0x18]; // 0x320(0x18)
	char TeamNum; // 0x338(0x01)
	char pad_339[0x1]; // 0x339(0x01)
	uint16_t ProgressTowardsScoring; // 0x33a(0x02)
	char pad_33C[0x1c]; // 0x33c(0x1c)
	char KillStreak; // 0x358(0x01)
	char MultiKillCounter; // 0x359(0x01)
	char pad_35A[0x5e]; // 0x35a(0x5e)
	char DamagePlayers[0x50]; // 0x3b8(0x50)
	char pad_408[0xf0]; // 0x408(0xf0)
	struct FPlayerStatsInfo_InDepth PlayerStats; // 0x4f8(0xc8)
	char PostGameStats[0x268]; // 0x5c0(0x268)
	char pad_828[0x18]; // 0x828(0x18)
	bool bIsAdmin; // 0x840(0x01)
	char pad_841[0x7]; // 0x841(0x07)
	struct FEquippedCustomizations PlayerCustomizations; // 0x848(0x10)
	char pad_858[0x30]; // 0x858(0x30)
	struct TArray<struct UObject*> CachedCustomizationObjects; // 0x888(0x10)
	char pad_898[0x38]; // 0x898(0x38)
	struct UTexture* PlayerAvatar; // 0x8d0(0x08)
	struct UTexture* SavedPlayerAvatar; // 0x8d8(0x08)
	struct UTexture* SavedBotAvatar; // 0x8e0(0x08)
	struct TArray<struct FUserRankInfo> PlayerRanks; // 0x8e8(0x10)
	char pad_8F8[0x18]; // 0x8f8(0x18)
	char Progression[0x10]; // 0x910(0x10)
	char pad_920[0x18]; // 0x920(0x18)
	char DailyStreak[0x18]; // 0x938(0x18)
	char pad_950[0x18]; // 0x950(0x18)
	char BadgeProgress[0x10]; // 0x968(0x10)
	char pad_978[0x50]; // 0x978(0x50)
	bool bHideNames; // 0x9c8(0x01)
	char bAnonymousMode : 1; // 0x9c9(0x01)
	char bPartyChatOnly : 1; // 0x9c9(0x01)
	char pad_9C9_2 : 6; // 0x9c9(0x01)
	char pad_9CA[0x26]; // 0x9ca(0x26)
	struct APortalWarsCharacter* CachedCharacter; // 0x9f0(0x08)
	char bQuitter : 1; // 0x9f8(0x01)
	char pad_9F8_1 : 7; // 0x9f8(0x01)
	char pad_9F9[0x1]; // 0x9f9(0x01)
	uint16_t InactiveTime; // 0x9fa(0x02)
	bool bJoinedInProgress; // 0x9fc(0x01)
	char pad_9FD[0x28]; // 0x9fd(0x28)
	bool bIsFlagged; // 0xa25(0x01)
	char pad_A26[0x1a]; // 0xa26(0x1a)
	bool bIsReadyForNextMatch; // 0xa40(0x01)
	char pad_A41[0x37]; // 0xa41(0x37)
	char DefaultProfile[0xc0]; // 0xa78(0xc0)
	float SimulatedFOV; // 0xb38(0x04)
	char pad_B3C[0x14]; // 0xb3c(0x14)

	void SetTeamNum(char NewTeamNumber); // Function PortalWars.PortalWarsPlayerState.SetTeamNum // (Final|Native|Public|BlueprintCallable) // @ game+0x16be440
	void SetPlayerAvatar(struct UTexture* avatar); // Function PortalWars.PortalWarsPlayerState.SetPlayerAvatar // (Final|Native|Public) // @ game+0x16be230
	void ServerUpdateLobbySessionId(struct FString NewLobbySessionId); // Function PortalWars.PortalWarsPlayerState.ServerUpdateLobbySessionId // (Net|NetReliableNative|Event|Protected|NetServer) // @ game+0x16be170
	void ServerChangeTeam(); // Function PortalWars.PortalWarsPlayerState.ServerChangeTeam // (Net|NetReliableNative|Event|Protected|NetServer|NetValidate) // @ game+0x16bd730
	void OnRep_Team(); // Function PortalWars.PortalWarsPlayerState.OnRep_Team // (Native|Protected) // @ game+0x169a850
	void OnRep_ReadyForNextMatch(); // Function PortalWars.PortalWarsPlayerState.OnRep_ReadyForNextMatch // (Final|Native|Protected) // @ game+0x16bd1a0
	void OnRep_Ranks(); // Function PortalWars.PortalWarsPlayerState.OnRep_Ranks // (Final|Native|Protected) // @ game+0x16bd180
	void OnRep_Progression(); // Function PortalWars.PortalWarsPlayerState.OnRep_Progression // (Final|Native|Protected) // @ game+0x16bd160
	void OnRep_PlayerStats(); // Function PortalWars.PortalWarsPlayerState.OnRep_PlayerStats // (Final|Native|Protected) // @ game+0x16bd140
	void OnRep_PlayerCustomizations(); // Function PortalWars.PortalWarsPlayerState.OnRep_PlayerCustomizations // (Final|Native|Protected) // @ game+0x16bd120
	void OnRep_PartyChatOnly(); // Function PortalWars.PortalWarsPlayerState.OnRep_PartyChatOnly // (Final|Native|Protected) // @ game+0x16bd100
	void OnRep_MultiKillCounter(char OldMultiKillCounter); // Function PortalWars.PortalWarsPlayerState.OnRep_MultiKillCounter // (Final|Native|Protected) // @ game+0x16bd080
	void OnRep_KillStreak(char OldKillStreak); // Function PortalWars.PortalWarsPlayerState.OnRep_KillStreak // (Final|Native|Protected) // @ game+0x16bd000
	void OnRep_JoinedInProgress(); // Function PortalWars.PortalWarsPlayerState.OnRep_JoinedInProgress // (Final|Native|Protected) // @ game+0x16bcfe0
	void OnRep_IsFlagged(); // Function PortalWars.PortalWarsPlayerState.OnRep_IsFlagged // (Final|Native|Protected) // @ game+0x16bcfc0
	void OnRep_IsAdmin(); // Function PortalWars.PortalWarsPlayerState.OnRep_IsAdmin // (Final|Native|Protected) // @ game+0x16bcfa0
	void OnRep_DefaultProfile(); // Function PortalWars.PortalWarsPlayerState.OnRep_DefaultProfile // (Final|Native|Protected) // @ game+0x16bcf80
	void OnRep_DailyStreak(); // Function PortalWars.PortalWarsPlayerState.OnRep_DailyStreak // (Final|Native|Protected) // @ game+0x16bcf60
	void OnRep_BadgeProgress(); // Function PortalWars.PortalWarsPlayerState.OnRep_BadgeProgress // (Final|Native|Protected) // @ game+0x16bcf40
	void OnRep_AnonymousMode(); // Function PortalWars.PortalWarsPlayerState.OnRep_AnonymousMode // (Final|Native|Protected) // @ game+0x16bcf20
	void OnPlayerAvatarLoaded(struct UTexture* avatar); // Function PortalWars.PortalWarsPlayerState.OnPlayerAvatarLoaded // (Final|Native|Public) // @ game+0x16bcda0
	void OnDefaultAvatarLoaded(); // Function PortalWars.PortalWarsPlayerState.OnDefaultAvatarLoaded // (Final|Native|Protected) // @ game+0x16bcd20
	void OnCustomizationsLoaded(); // Function PortalWars.PortalWarsPlayerState.OnCustomizationsLoaded // (Final|Native|Protected) // @ game+0x16bcd00
	void LoadPlayerAvatar(); // Function PortalWars.PortalWarsPlayerState.LoadPlayerAvatar // (Final|Native|Protected) // @ game+0x16bcc80
	char GetTeamNum(bool bUseKillerDuringKillcam); // Function PortalWars.PortalWarsPlayerState.GetTeamNum // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x16bcb80
	struct FString GetShortPlayerName(); // Function PortalWars.PortalWarsPlayerState.GetShortPlayerName // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x16bca30
	void ClientTriggerEvents(struct TArray<struct FName> EventNames); // Function PortalWars.PortalWarsPlayerState.ClientTriggerEvents // (Net|Native|Event|Protected|NetClient) // @ game+0x16bc510
	void ClientTriggerEvent(struct FName EventName); // Function PortalWars.PortalWarsPlayerState.ClientTriggerEvent // (Net|Native|Event|Protected|NetClient) // @ game+0x16bc480
	void ClientReconnected(); // Function PortalWars.PortalWarsPlayerState.ClientReconnected // (Net|NetReliableNative|Event|Protected|NetClient) // @ game+0x1695cd0
	void ClientReceivedMedals(struct TArray<struct FName> MedalNames); // Function PortalWars.PortalWarsPlayerState.ClientReceivedMedals // (Net|Native|Event|Protected|NetClient) // @ game+0x16bc080
	void ClientReceivedMedal(struct FName MedalName); // Function PortalWars.PortalWarsPlayerState.ClientReceivedMedal // (Net|Native|Event|Protected|NetClient) // @ game+0x16bbff0
	void BroadcastDeath_Multicast(struct APortalWarsPlayerState* KillerPlayerState, struct UDamageType* KillerDamageType, struct APortalWarsPlayerState* KilledPlayerState, bool bIsHeadshot, bool bThruPortal, float KillerLastTeleportTime, struct TArray<struct APortalWarsPlayerState*> DamagePlayerStates, struct TArray<float> DamagePlayerDamages); // Function PortalWars.PortalWarsPlayerState.BroadcastDeath_Multicast // (Net|NetReliableNative|Event|NetMulticast|Public) // @ game+0x16bb600
};

// Enum CoreUObject.EMouseCursor
enum class EMouseCursor : uint8_t {
	None = 0,
	Default = 1,
	TextEditBeam = 2,
	ResizeLeftRight = 3,
	ResizeUpDown = 4,
	ResizeSouthEast = 5,
	ResizeSouthWest = 6,
	CardinalCross = 7,
	Crosshairs = 8,
	Hand = 9,
	GrabHand = 10,
	GrabHandClosed = 11,
	SlashedCircle = 12,
	EyeDropper = 13,
	EMouseCursor_MAX = 14
};

// Class Engine.PlayerController
struct APlayerController : AController {
	struct UPlayer* Player; // 0x298(0x08)
	struct APawn* AcknowledgedPawn; // 0x2a0(0x08)
	struct UInterpTrackInstDirector* ControllingDirTrackInst; // 0x2a8(0x08)
	struct AHUD* MyHUD; // 0x2b0(0x08)
	struct APlayerCameraManager* PlayerCameraManager; // 0x2b8(0x08)
	struct APlayerCameraManager* PlayerCameraManagerClass; // 0x2c0(0x08)
	bool bAutoManageActiveCameraTarget; // 0x2c8(0x01)
	char pad_2C9[0x3]; // 0x2c9(0x03)
	struct FRotator TargetViewRotation; // 0x2cc(0x0c)
	char pad_2D8[0xc]; // 0x2d8(0x0c)
	float SmoothTargetViewRotationSpeed; // 0x2e4(0x04)
	char pad_2E8[0x8]; // 0x2e8(0x08)
	struct TArray<struct AActor*> HiddenActors; // 0x2f0(0x10)
	char HiddenPrimitiveComponents[0x10]; // 0x300(0x10)
	char pad_310[0x4]; // 0x310(0x04)
	float LastSpectatorStateSynchTime; // 0x314(0x04)
	struct FVector LastSpectatorSyncLocation; // 0x318(0x0c)
	struct FRotator LastSpectatorSyncRotation; // 0x324(0x0c)
	int32_t ClientCap; // 0x330(0x04)
	char pad_334[0x4]; // 0x334(0x04)
	struct UCheatManager* CheatManager; // 0x338(0x08)
	struct UCheatManager* CheatClass; // 0x340(0x08)
	struct UPlayerInput* PlayerInput; // 0x348(0x08)
	struct TArray<struct FActiveForceFeedbackEffect> ActiveForceFeedbackEffects; // 0x350(0x10)
	char pad_360[0x70]; // 0x360(0x70)
	char pad_3D0_0 : 4; // 0x3d0(0x01)
	char bPlayerIsWaiting : 1; // 0x3d0(0x01)
	char pad_3D0_5 : 3; // 0x3d0(0x01)
	char pad_3D1[0x3]; // 0x3d1(0x03)
	char NetPlayerIndex; // 0x3d4(0x01)
	char pad_3D5[0x3b]; // 0x3d5(0x3b)
	struct UNetConnection* PendingSwapConnection; // 0x410(0x08)
	struct UNetConnection* NetConnection; // 0x418(0x08)
	char pad_420[0xc]; // 0x420(0x0c)
	float InputYawScale; // 0x42c(0x04)
	float InputPitchScale; // 0x430(0x04)
	float InputRollScale; // 0x434(0x04)
	char bShowMouseCursor : 1; // 0x438(0x01)
	char bEnableClickEvents : 1; // 0x438(0x01)
	char bEnableTouchEvents : 1; // 0x438(0x01)
	char bEnableMouseOverEvents : 1; // 0x438(0x01)
	char bEnableTouchOverEvents : 1; // 0x438(0x01)
	char bForceFeedbackEnabled : 1; // 0x438(0x01)
	char pad_438_6 : 2; // 0x438(0x01)
	char pad_439[0x3]; // 0x439(0x03)
	float ForceFeedbackScale; // 0x43c(0x04)
	struct TArray<struct FKey> ClickEventKeys; // 0x440(0x10)
	enum class EMouseCursor DefaultMouseCursor; // 0x450(0x01)
	enum class EMouseCursor CurrentMouseCursor; // 0x451(0x01)
	char DefaultClickTraceChannel[0x01]; // 0x452(0x01)
	char CurrentClickTraceChannel[0x01]; // 0x453(0x01)
	float HitResultTraceDistance; // 0x454(0x04)
	uint16_t SeamlessTravelCount; // 0x458(0x02)
	uint16_t LastCompletedSeamlessTravelCount; // 0x45a(0x02)
	char pad_45C[0x74]; // 0x45c(0x74)
	struct UInputComponent* InactiveStateInputComponent; // 0x4d0(0x08)
	char pad_4D8_0 : 2; // 0x4d8(0x01)
	char bShouldPerformFullTickWhenPaused : 1; // 0x4d8(0x01)
	char pad_4D8_3 : 5; // 0x4d8(0x01)
	char pad_4D9[0x17]; // 0x4d9(0x17)
	struct UTouchInterface* CurrentTouchInterface; // 0x4f0(0x08)
	char pad_4F8[0x50]; // 0x4f8(0x50)
	struct ASpectatorPawn* SpectatorPawn; // 0x548(0x08)
	char pad_550[0x4]; // 0x550(0x04)
	bool bIsLocalPlayerController; // 0x554(0x01)
	char pad_555[0x3]; // 0x555(0x03)
	struct FVector SpawnLocation; // 0x558(0x0c)
	char pad_564[0xc]; // 0x564(0x0c)

	bool ProjectWorldLocationToScreen(FVector& WorldLocation, FVector2D& ScreenLocation);
	void GetViewportSize(INT& X, INT& Y);
	void SwitchLevel(struct FString URL); // Function Engine.PlayerController.SwitchLevel // (Exec|Native|Public) // @ game+0x37b6640
	void FOV(float NewFOV); // Function Engine.PlayerController.FOV // (Exec|Native|Public) // @ game+0x37b1830
	void EnableCheats(); // Function Engine.PlayerController.EnableCheats // (Exec|Native|Public) // @ game+0x37b1810
	void SetName(FString S); // Function Engine.PlayerController.SetName // (Exec|Native|Public) // @ game+0x37b5690
	void SendToConsole(struct FString Command); // Function Engine.PlayerController.SendToConsole // (Exec|Native|Public) // @ game+0x37b3d60
	void ConsoleKey(struct FKey Key); // Function Engine.PlayerController.ConsoleKey // (Exec|Native|Public) // @ game+0x37b1490
	void Camera(struct FName NewMode); // Function Engine.PlayerController.Camera // (Exec|Native|Public) // @ game+0x37ae830
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

	static struct UWorld* GetWorld();
	static struct UClass* StaticClass();
	static struct UWorld* GetDefaultObj();
};

// Class Engine.Canvas
struct Canvas : UObject {
	char pad_0000[0x00]; // 0x28
	void K2_DrawLine(FVector2D ScreenPositionA, FVector2D ScreenPositionB, FLOAT Thickness, FLinearColor Color);
	void K2_DrawText(FString RenderText, FVector2D ScreenPosition, FVector2D Scale, FLinearColor RenderColor, float Kerning, FLinearColor ShadowColor, FVector2D ShadowOffset, bool bCentreX, bool bCentreY, bool bOutlined, FLinearColor OutlineColor);
};

// Class Engine.ScriptViewportClient
// Size: 0x38 (Inherited: 0x28)
struct UScriptViewportClient : UObject {
	char pad_28[0x10]; // 0x28(0x10)
};

// Class Engine.GameViewportClient
// Size: 0x360 (Inherited: 0x38)
struct UGameViewportClient : UScriptViewportClient {
	char pad_38[0x8]; // 0x38(0x08)
	struct UConsole* ViewportConsole; // 0x40(0x08)
	struct TArray<struct FDebugDisplayProperty> DebugProperties; // 0x48(0x10)
	char pad_58[0x10]; // 0x58(0x10)
	int32_t MaxSplitscreenPlayers; // 0x68(0x04)
	char pad_6C[0xc]; // 0x6c(0x0c)
	struct UWorld* World; // 0x78(0x08)
	struct UGameInstance* GameInstance; // 0x80(0x08)
	char pad_88[0x2d8]; // 0x88(0x2d8)

	void SSSwapControllers(); // Function Engine.GameViewportClient.SSSwapControllers // (Exec|Native|Public) // @ game+0x102e110
	void ShowTitleSafeArea(); // Function Engine.GameViewportClient.ShowTitleSafeArea // (Exec|Native|Public) // @ game+0x27d8180
	void SetConsoleTarget(int32_t PlayerIndex); // Function Engine.GameViewportClient.SetConsoleTarget // (Exec|Native|Public) // @ game+0x37031e0
};

// Class Engine.BlueprintFunctionLibrary
// Size: 0x28 (Inherited: 0x28)
struct UBlueprintFunctionLibrary : UObject {
};

// Class Engine.GameplayStatics
// Size: 0x28 (Inherited: 0x28)
struct UGameplayStatics : UBlueprintFunctionLibrary {
	struct UObject* SpawnObject(struct UObject* ObjectClass, struct UObject* Outer); // Function Engine.GameplayStatics.SpawnObject // (Final|Native|Static|Public|BlueprintCallable) // @ game+0x36f9330

	static struct UClass* StaticClass();
};

// ScriptStruct CoreUObject.SoftObjectPath
// Size: 0x18 (Inherited: 0x00)
struct FSoftObjectPath {
	struct FName AssetPathName; // 0x00(0x08)
	struct FString SubPathString; // 0x08(0x10)
};

// ScriptStruct CoreUObject.FloatRangeBound
// Size: 0x08 (Inherited: 0x00)
struct FFloatRangeBound {
	char pad_0[0x01]; // 0x00(0x01)
	char pad_1[0x3]; // 0x01(0x03)
	float Value; // 0x04(0x04)
};

// ScriptStruct CoreUObject.FloatRange
// Size: 0x10 (Inherited: 0x00)
struct FFloatRange {
	struct FFloatRangeBound LowerBound; // 0x00(0x08)
	struct FFloatRangeBound UpperBound; // 0x08(0x08)
};


// ScriptStruct CoreUObject.SoftClassPath
// Size: 0x18 (Inherited: 0x18)
struct FSoftClassPath : FSoftObjectPath {
};

// ScriptStruct Engine.DirectoryPath
// Size: 0x10 (Inherited: 0x00)
struct FDirectoryPath {
	struct FString Path; // 0x00(0x10)
};

// ScriptStruct CoreUObject.FrameRate
// Size: 0x08 (Inherited: 0x00)
struct FFrameRate {
	int32_t Numerator; // 0x00(0x04)
	int32_t Denominator; // 0x04(0x04)
};

// Class Engine.Engine
// Size: 0xd20 (Inherited: 0x28)
struct UEngine : UObject {
	char pad_28[0x8]; // 0x28(0x08)
	struct UFont* TinyFont; // 0x30(0x08)
	struct FSoftObjectPath TinyFontName; // 0x38(0x18)
	struct UFont* SmallFont; // 0x50(0x08)
	struct FSoftObjectPath SmallFontName; // 0x58(0x18)
	struct UFont* MediumFont; // 0x70(0x08)
	struct FSoftObjectPath MediumFontName; // 0x78(0x18)
	struct UFont* LargeFont; // 0x90(0x08)
	struct FSoftObjectPath LargeFontName; // 0x98(0x18)
	struct UFont* SubtitleFont; // 0xb0(0x08)
	struct FSoftObjectPath SubtitleFontName; // 0xb8(0x18)
	struct TArray<struct UFont*> AdditionalFonts; // 0xd0(0x10)
	struct TArray<struct FString> AdditionalFontNames; // 0xe0(0x10)
	struct UConsole* ConsoleClass; // 0xf0(0x08)
	struct FSoftClassPath ConsoleClassName; // 0xf8(0x18)
	struct UGameViewportClient* GameViewportClientClass; // 0x110(0x08)
	struct FSoftClassPath GameViewportClientClassName; // 0x118(0x18)
	struct ULocalPlayer* LocalPlayerClass; // 0x130(0x08)
	struct FSoftClassPath LocalPlayerClassName; // 0x138(0x18)
	struct AWorldSettings* WorldSettingsClass; // 0x150(0x08)
	struct FSoftClassPath WorldSettingsClassName; // 0x158(0x18)
	struct FSoftClassPath NavigationSystemClassName; // 0x170(0x18)
	struct UNavigationSystemBase* NavigationSystemClass; // 0x188(0x08)
	struct FSoftClassPath NavigationSystemConfigClassName; // 0x190(0x18)
	struct UNavigationSystemConfig* NavigationSystemConfigClass; // 0x1a8(0x08)
	struct FSoftClassPath AvoidanceManagerClassName; // 0x1b0(0x18)
	struct UAvoidanceManager* AvoidanceManagerClass; // 0x1c8(0x08)
	struct FSoftClassPath AIControllerClassName; // 0x1d0(0x18)
	struct UPhysicsCollisionHandler* PhysicsCollisionHandlerClass; // 0x1e8(0x08)
	struct FSoftClassPath PhysicsCollisionHandlerClassName; // 0x1f0(0x18)
	struct FSoftClassPath GameUserSettingsClassName; // 0x208(0x18)
	struct UGameUserSettings* GameUserSettingsClass; // 0x220(0x08)
	struct UGameUserSettings* GameUserSettings; // 0x228(0x08)
	struct ALevelScriptActor* LevelScriptActorClass; // 0x230(0x08)
	struct FSoftClassPath LevelScriptActorClassName; // 0x238(0x18)
	struct FSoftClassPath DefaultBlueprintBaseClassName; // 0x250(0x18)
	struct FSoftClassPath GameSingletonClassName; // 0x268(0x18)
	struct UObject* GameSingleton; // 0x280(0x08)
	struct FSoftClassPath AssetManagerClassName; // 0x288(0x18)
	struct UAssetManager* AssetManager; // 0x2a0(0x08)
	struct UTexture2D* DefaultTexture; // 0x2a8(0x08)
	struct FSoftObjectPath DefaultTextureName; // 0x2b0(0x18)
	struct UTexture* DefaultDiffuseTexture; // 0x2c8(0x08)
	struct FSoftObjectPath DefaultDiffuseTextureName; // 0x2d0(0x18)
	struct UTexture2D* DefaultBSPVertexTexture; // 0x2e8(0x08)
	struct FSoftObjectPath DefaultBSPVertexTextureName; // 0x2f0(0x18)
	struct UTexture2D* HighFrequencyNoiseTexture; // 0x308(0x08)
	struct FSoftObjectPath HighFrequencyNoiseTextureName; // 0x310(0x18)
	struct UTexture2D* DefaultBokehTexture; // 0x328(0x08)
	struct FSoftObjectPath DefaultBokehTextureName; // 0x330(0x18)
	struct UTexture2D* DefaultBloomKernelTexture; // 0x348(0x08)
	struct FSoftObjectPath DefaultBloomKernelTextureName; // 0x350(0x18)
	struct UMaterial* WireframeMaterial; // 0x368(0x08)
	struct FString WireframeMaterialName; // 0x370(0x10)
	struct UMaterial* DebugMeshMaterial; // 0x380(0x08)
	struct FSoftObjectPath DebugMeshMaterialName; // 0x388(0x18)
	struct UMaterial* EmissiveMeshMaterial; // 0x3a0(0x08)
	struct FSoftObjectPath EmissiveMeshMaterialName; // 0x3a8(0x18)
	struct UMaterial* LevelColorationLitMaterial; // 0x3c0(0x08)
	struct FString LevelColorationLitMaterialName; // 0x3c8(0x10)
	struct UMaterial* LevelColorationUnlitMaterial; // 0x3d8(0x08)
	struct FString LevelColorationUnlitMaterialName; // 0x3e0(0x10)
	struct UMaterial* LightingTexelDensityMaterial; // 0x3f0(0x08)
	struct FString LightingTexelDensityName; // 0x3f8(0x10)
	struct UMaterial* ShadedLevelColorationLitMaterial; // 0x408(0x08)
	struct FString ShadedLevelColorationLitMaterialName; // 0x410(0x10)
	struct UMaterial* ShadedLevelColorationUnlitMaterial; // 0x420(0x08)
	struct FString ShadedLevelColorationUnlitMaterialName; // 0x428(0x10)
	struct UMaterial* RemoveSurfaceMaterial; // 0x438(0x08)
	struct FSoftObjectPath RemoveSurfaceMaterialName; // 0x440(0x18)
	struct UMaterial* VertexColorMaterial; // 0x458(0x08)
	struct FString VertexColorMaterialName; // 0x460(0x10)
	struct UMaterial* VertexColorViewModeMaterial_ColorOnly; // 0x470(0x08)
	struct FString VertexColorViewModeMaterialName_ColorOnly; // 0x478(0x10)
	struct UMaterial* VertexColorViewModeMaterial_AlphaAsColor; // 0x488(0x08)
	struct FString VertexColorViewModeMaterialName_AlphaAsColor; // 0x490(0x10)
	struct UMaterial* VertexColorViewModeMaterial_RedOnly; // 0x4a0(0x08)
	struct FString VertexColorViewModeMaterialName_RedOnly; // 0x4a8(0x10)
	struct UMaterial* VertexColorViewModeMaterial_GreenOnly; // 0x4b8(0x08)
	struct FString VertexColorViewModeMaterialName_GreenOnly; // 0x4c0(0x10)
	struct UMaterial* VertexColorViewModeMaterial_BlueOnly; // 0x4d0(0x08)
	struct FString VertexColorViewModeMaterialName_BlueOnly; // 0x4d8(0x10)
	struct FSoftObjectPath DebugEditorMaterialName; // 0x4e8(0x18)
	struct UMaterial* ConstraintLimitMaterial; // 0x500(0x08)
	struct UMaterialInstanceDynamic* ConstraintLimitMaterialX; // 0x508(0x08)
	struct UMaterialInstanceDynamic* ConstraintLimitMaterialXAxis; // 0x510(0x08)
	struct UMaterialInstanceDynamic* ConstraintLimitMaterialY; // 0x518(0x08)
	struct UMaterialInstanceDynamic* ConstraintLimitMaterialYAxis; // 0x520(0x08)
	struct UMaterialInstanceDynamic* ConstraintLimitMaterialZ; // 0x528(0x08)
	struct UMaterialInstanceDynamic* ConstraintLimitMaterialZAxis; // 0x530(0x08)
	struct UMaterialInstanceDynamic* ConstraintLimitMaterialPrismatic; // 0x538(0x08)
	struct UMaterial* InvalidLightmapSettingsMaterial; // 0x540(0x08)
	struct FSoftObjectPath InvalidLightmapSettingsMaterialName; // 0x548(0x18)
	struct UMaterial* PreviewShadowsIndicatorMaterial; // 0x560(0x08)
	struct FSoftObjectPath PreviewShadowsIndicatorMaterialName; // 0x568(0x18)
	struct UMaterial* ArrowMaterial; // 0x580(0x08)
	struct UMaterialInstanceDynamic* ArrowMaterialYellow; // 0x588(0x08)
	struct FSoftObjectPath ArrowMaterialName; // 0x590(0x18)
	struct FLinearColor LightingOnlyBrightness; // 0x5a8(0x10)
	struct TArray<struct FLinearColor> ShaderComplexityColors; // 0x5b8(0x10)
	struct TArray<struct FLinearColor> QuadComplexityColors; // 0x5c8(0x10)
	struct TArray<struct FLinearColor> LightComplexityColors; // 0x5d8(0x10)
	struct TArray<struct FLinearColor> StationaryLightOverlapColors; // 0x5e8(0x10)
	struct TArray<struct FLinearColor> LODColorationColors; // 0x5f8(0x10)
	struct TArray<struct FLinearColor> HLODColorationColors; // 0x608(0x10)
	struct TArray<struct FLinearColor> StreamingAccuracyColors; // 0x618(0x10)
	float MaxPixelShaderAdditiveComplexityCount; // 0x628(0x04)
	float MaxES3PixelShaderAdditiveComplexityCount; // 0x62c(0x04)
	float MinLightMapDensity; // 0x630(0x04)
	float IdealLightMapDensity; // 0x634(0x04)
	float MaxLightMapDensity; // 0x638(0x04)
	char bRenderLightMapDensityGrayscale : 1; // 0x63c(0x01)
	char pad_63C_1 : 7; // 0x63c(0x01)
	char pad_63D[0x3]; // 0x63d(0x03)
	float RenderLightMapDensityGrayscaleScale; // 0x640(0x04)
	float RenderLightMapDensityColorScale; // 0x644(0x04)
	struct FLinearColor LightMapDensityVertexMappedColor; // 0x648(0x10)
	struct FLinearColor LightMapDensitySelectedColor; // 0x658(0x10)
	struct TArray<struct FStatColorMapping> StatColorMappings; // 0x668(0x10)
	struct UPhysicalMaterial* DefaultPhysMaterial; // 0x678(0x08)
	struct FSoftObjectPath DefaultPhysMaterialName; // 0x680(0x18)
	struct TArray<struct FGameNameRedirect> ActiveGameNameRedirects; // 0x698(0x10)
	struct TArray<struct FClassRedirect> ActiveClassRedirects; // 0x6a8(0x10)
	struct TArray<struct FPluginRedirect> ActivePluginRedirects; // 0x6b8(0x10)
	struct TArray<struct FStructRedirect> ActiveStructRedirects; // 0x6c8(0x10)
	struct UTexture2D* PreIntegratedSkinBRDFTexture; // 0x6d8(0x08)
	struct FSoftObjectPath PreIntegratedSkinBRDFTextureName; // 0x6e0(0x18)
	struct UTexture2D* BlueNoiseTexture; // 0x6f8(0x08)
	struct FSoftObjectPath BlueNoiseTextureName; // 0x700(0x18)
	struct UTexture2D* MiniFontTexture; // 0x718(0x08)
	struct FSoftObjectPath MiniFontTextureName; // 0x720(0x18)
	struct UTexture* WeightMapPlaceholderTexture; // 0x738(0x08)
	struct FSoftObjectPath WeightMapPlaceholderTextureName; // 0x740(0x18)
	struct UTexture2D* LightMapDensityTexture; // 0x758(0x08)
	struct FSoftObjectPath LightMapDensityTextureName; // 0x760(0x18)
	char pad_778[0x8]; // 0x778(0x08)
	struct UGameViewportClient* GameViewport; // 0x780(0x08)
	struct TArray<struct FString> DeferredCommands; // 0x788(0x10)
	float NearClipPlane; // 0x798(0x04)
	char bSubtitlesEnabled : 1; // 0x79c(0x01)
	char bSubtitlesForcedOff : 1; // 0x79c(0x01)
	char pad_79C_2 : 6; // 0x79c(0x01)
	char pad_79D[0x3]; // 0x79d(0x03)
	int32_t MaximumLoopIterationCount; // 0x7a0(0x04)
	char bCanBlueprintsTickByDefault : 1; // 0x7a4(0x01)
	char bOptimizeAnimBlueprintMemberVariableAccess : 1; // 0x7a4(0x01)
	char bAllowMultiThreadedAnimationUpdate : 1; // 0x7a4(0x01)
	char bEnableEditorPSysRealtimeLOD : 1; // 0x7a4(0x01)
	char pad_7A4_4 : 1; // 0x7a4(0x01)
	char bSmoothFrameRate : 1; // 0x7a4(0x01)
	char bUseFixedFrameRate : 1; // 0x7a4(0x01)
	char pad_7A4_7 : 1; // 0x7a4(0x01)
	char pad_7A5[0x3]; // 0x7a5(0x03)
	float FixedFrameRate; // 0x7a8(0x04)
	struct FFloatRange SmoothedFrameRateRange; // 0x7ac(0x10)
	char pad_7BC[0x4]; // 0x7bc(0x04)
	struct UEngineCustomTimeStep* CustomTimeStep; // 0x7c0(0x08)
	char pad_7C8[0x20]; // 0x7c8(0x20)
	struct FSoftClassPath CustomTimeStepClassName; // 0x7e8(0x18)
	struct UTimecodeProvider* TimecodeProvider; // 0x800(0x08)
	char pad_808[0x20]; // 0x808(0x20)
	struct FSoftClassPath TimecodeProviderClassName; // 0x828(0x18)
	bool bGenerateDefaultTimecode; // 0x840(0x01)
	char pad_841[0x3]; // 0x841(0x03)
	struct FFrameRate GenerateDefaultTimecodeFrameRate; // 0x844(0x08)
	float GenerateDefaultTimecodeFrameDelay; // 0x84c(0x04)
	char bCheckForMultiplePawnsSpawnedInAFrame : 1; // 0x850(0x01)
	char pad_850_1 : 7; // 0x850(0x01)
	char pad_851[0x3]; // 0x851(0x03)
	int32_t NumPawnsAllowedToBeSpawnedInAFrame; // 0x854(0x04)
	char bShouldGenerateLowQualityLightmaps : 1; // 0x858(0x01)
	char pad_858_1 : 7; // 0x858(0x01)
	char pad_859[0x3]; // 0x859(0x03)
	struct FColor C_WorldBox; // 0x85c(0x04)
	struct FColor C_BrushWire; // 0x860(0x04)
	struct FColor C_AddWire; // 0x864(0x04)
	struct FColor C_SubtractWire; // 0x868(0x04)
	struct FColor C_SemiSolidWire; // 0x86c(0x04)
	struct FColor C_NonSolidWire; // 0x870(0x04)
	struct FColor C_WireBackground; // 0x874(0x04)
	struct FColor C_ScaleBoxHi; // 0x878(0x04)
	struct FColor C_VolumeCollision; // 0x87c(0x04)
	struct FColor C_BSPCollision; // 0x880(0x04)
	struct FColor C_OrthoBackground; // 0x884(0x04)
	struct FColor C_Volume; // 0x888(0x04)
	struct FColor C_BrushShape; // 0x88c(0x04)
	float StreamingDistanceFactor; // 0x890(0x04)
	char pad_894[0x4]; // 0x894(0x04)
	struct FDirectoryPath GameScreenshotSaveDirectory; // 0x898(0x10)
	char pad_8a8[0x01]; // 0x8a8(0x01)
	char pad_8A9[0x7]; // 0x8a9(0x07)
	struct FString TransitionDescription; // 0x8b0(0x10)
	struct FString TransitionGameMode; // 0x8c0(0x10)
	char bAllowMatureLanguage : 1; // 0x8d0(0x01)
	char pad_8D0_1 : 7; // 0x8d0(0x01)
	char pad_8D1[0x3]; // 0x8d1(0x03)
	float CameraRotationThreshold; // 0x8d4(0x04)
	float CameraTranslationThreshold; // 0x8d8(0x04)
	float PrimitiveProbablyVisibleTime; // 0x8dc(0x04)
	float MaxOcclusionPixelsFraction; // 0x8e0(0x04)
	char bPauseOnLossOfFocus : 1; // 0x8e4(0x01)
	char pad_8E4_1 : 7; // 0x8e4(0x01)
	char pad_8E5[0x3]; // 0x8e5(0x03)
	int32_t MaxParticleResize; // 0x8e8(0x04)
	int32_t MaxParticleResizeWarn; // 0x8ec(0x04)
	struct TArray<struct FDropNoteInfo> PendingDroppedNotes; // 0x8f0(0x10)
	float NetClientTicksPerSecond; // 0x900(0x04)
	float DisplayGamma; // 0x904(0x04)
	float MinDesiredFrameRate; // 0x908(0x04)
	struct FLinearColor DefaultSelectedMaterialColor; // 0x90c(0x10)
	struct FLinearColor SelectedMaterialColor; // 0x91c(0x10)
	struct FLinearColor SelectionOutlineColor; // 0x92c(0x10)
	struct FLinearColor SubduedSelectionOutlineColor; // 0x93c(0x10)
	struct FLinearColor SelectedMaterialColorOverride; // 0x94c(0x10)
	bool bIsOverridingSelectedColor; // 0x95c(0x01)
	char pad_95D[0x3]; // 0x95d(0x03)
	char bEnableOnScreenDebugMessages : 1; // 0x960(0x01)
	char bEnableOnScreenDebugMessagesDisplay : 1; // 0x960(0x01)
	char bSuppressMapWarnings : 1; // 0x960(0x01)
	char bDisableAILogging : 1; // 0x960(0x01)
	char pad_960_4 : 4; // 0x960(0x01)
	char pad_961[0x3]; // 0x961(0x03)
	uint32_t bEnableVisualLogRecordingOnStart; // 0x964(0x04)
	int32_t ScreenSaverInhibitorSemaphore; // 0x968(0x04)
	char bLockReadOnlyLevels : 1; // 0x96c(0x01)
	char pad_96C_1 : 7; // 0x96c(0x01)
	char pad_96D[0x3]; // 0x96d(0x03)
	struct FString ParticleEventManagerClassPath; // 0x970(0x10)
	float SelectionHighlightIntensity; // 0x980(0x04)
	float BSPSelectionHighlightIntensity; // 0x984(0x04)
	float SelectionHighlightIntensityBillboards; // 0x988(0x04)
	char pad_98C[0x26c]; // 0x98c(0x26c)
	struct TArray<struct FNetDriverDefinition> NetDriverDefinitions; // 0xbf8(0x10)
	struct TArray<struct FString> ServerActors; // 0xc08(0x10)
	struct TArray<struct FString> RuntimeServerActors; // 0xc18(0x10)
	float NetErrorLogInterval; // 0xc28(0x04)
	char bStartedLoadMapMovie : 1; // 0xc2c(0x01)
	char pad_C2C_1 : 7; // 0xc2c(0x01)
	char pad_C2D[0x1b]; // 0xc2d(0x1b)
	int32_t NextWorldContextHandle; // 0xc48(0x04)
	char pad_C4C[0xd4]; // 0xc4c(0xd4)

	static struct UEngine* GetEngine();
	static struct UClass* StaticClass();
	static struct UEngine* GetDefaultObj();
};

// ScriptStruct InputCore.Key
// Size: 0x18 (Inherited: 0x00)
struct FKey {
	struct FName KeyName; // 0x00(0x08)
	char pad_8[0x10]; // 0x08(0x10)
};

// Class Engine.InputSettings
// Size: 0x140 (Inherited: 0x28)
struct UInputSettings : UObject {
	struct TArray<struct FKey> ConsoleKeys; // 0x130(0x10)

	struct UInputSettings* GetInputSettings(); // Function Engine.InputSettings.GetInputSettings // (Final|Native|Static|Public|BlueprintCallable|BlueprintPure) // @ game+0x3707f30

	static struct UClass* StaticClass();
	static struct UInputSettings* GetDefaultObj();
};

// Class Engine.Console
// Size: 0x130 (Inherited: 0x28)
struct UConsole : UObject {
	char pad_28[0x10]; // 0x28(0x10)
	struct ULocalPlayer* ConsoleTargetPlayer; // 0x38(0x08)
	struct UTexture2D* DefaultTexture_Black; // 0x40(0x08)
	struct UTexture2D* DefaultTexture_White; // 0x48(0x08)
	char pad_50[0x18]; // 0x50(0x18)
	struct TArray<struct FString> HistoryBuffer; // 0x68(0x10)
	char pad_78[0xb8]; // 0x78(0xb8)

	static struct UClass* StaticClass();
};

// Class Engine.KismetStringLibrary
// Size: 0x28 (Inherited: 0x28)
struct UKismetStringLibrary : UBlueprintFunctionLibrary {
	struct FName Conv_StringToName(struct FString inString); // Function Engine.KismetStringLibrary.Conv_StringToName // (Final|Native|Static|Public|BlueprintCallable|BlueprintPure) // @ game+0x374bdb0

	static struct UClass* StaticClass();
};

extern FNamePool* NamePoolData;
extern TUObjectArray* ObjObjects;
extern UWorld* WRLD;

bool EngineInit();