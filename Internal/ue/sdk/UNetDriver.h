#pragma once

#include "Fwd.h"
#include "Values.h"
#include "UObject.h"

// Class Engine.NetDriver
// Size: 0x760 (Inherited: 0x28)
struct UNetDriver : UObject
{
	char pad_28[0x8];											 // 0x28(0x08)
	struct FString NetConnectionClassName;						 // 0x30(0x10)
	struct FString ReplicationDriverClassName;					 // 0x40(0x10)
	int32_t MaxDownloadSize;									 // 0x50(0x04)
	char bClampListenServerTickRate : 1;						 // 0x54(0x01)
	char pad_54_1 : 7;											 // 0x54(0x01)
	char pad_55[0x3];											 // 0x55(0x03)
	int32_t NetServerMaxTickRate;								 // 0x58(0x04)
	int32_t MaxNetTickRate;										 // 0x5c(0x04)
	int32_t MaxInternetClientRate;								 // 0x60(0x04)
	int32_t MaxClientRate;										 // 0x64(0x04)
	float ServerTravelPause;									 // 0x68(0x04)
	float SpawnPrioritySeconds;									 // 0x6c(0x04)
	float RelevantTimeout;										 // 0x70(0x04)
	float KeepAliveTime;										 // 0x74(0x04)
	float InitialConnectTimeout;								 // 0x78(0x04)
	float ConnectionTimeout;									 // 0x7c(0x04)
	float TimeoutMultiplierForUnoptimizedBuilds;				 // 0x80(0x04)
	bool bNoTimeouts;											 // 0x84(0x01)
	bool bNeverApplyNetworkEmulationSettings;					 // 0x85(0x01)
	char pad_86[0x2];											 // 0x86(0x02)
	struct UNetConnection* ServerConnection;					 // 0x88(0x08)
	struct TArray<struct UNetConnection*> ClientConnections;	 // 0x90(0x10)
	char pad_A0[0x60];											 // 0xa0(0x60)
	int32_t RecentlyDisconnectedTrackingTime;					 // 0x100(0x04)
	char pad_104[0x3c];											 // 0x104(0x3c)
	struct UWorld* World;										 // 0x140(0x08)
	struct UPackage* WorldPackage;								 // 0x148(0x08)
	char pad_150[0x20];											 // 0x150(0x20)
	struct UObject* NetConnectionClass;							 // 0x170(0x08)
	struct UObject* ReplicationDriverClass;						 // 0x178(0x08)
	char pad_180[0x10];											 // 0x180(0x10)
	struct FName NetDriverName;									 // 0x190(0x08)
	struct TArray<struct FChannelDefinition> ChannelDefinitions; // 0x198(0x10)
	char ChannelDefinitionMap[0x50];							 // 0x1a8(0x50)
	struct TArray<struct UChannel*> ActorChannelPool;			 // 0x1f8(0x10)
	char pad_208[0x8];											 // 0x208(0x08)
	float Time;													 // 0x210(0x04)
	char pad_214[0x4ec];										 // 0x214(0x4ec)
	struct UReplicationDriver* ReplicationDriver;				 // 0x700(0x08)
	char pad_708[0x58];											 // 0x708(0x58)
};
