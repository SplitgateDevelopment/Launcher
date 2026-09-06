#pragma once

#include "Fwd.h"
#include "Values.h"
#include "UPlayer.h"

// Class Engine.NetConnection
// Size: 0x1ba8 (Inherited: 0x48)
struct UNetConnection : UPlayer
{
	struct TArray<struct UChildConnection*> Children; // 0x48(0x10)
	struct UNetDriver* Driver;						  // 0x58(0x08)
	struct UPackageMap* PackageMapClass;			  // 0x60(0x08)
	struct UPackageMap* PackageMap;					  // 0x68(0x08)
	struct TArray<struct UChannel*> OpenChannels;	  // 0x70(0x10)
	struct TArray<struct AActor*> SentTemporaries;	  // 0x80(0x10)
	struct AActor* ViewTarget;						  // 0x90(0x08)
	struct AActor* OwningActor;						  // 0x98(0x08)
	int32_t MaxPacket;								  // 0xa0(0x04)
	char InternalAck : 1;							  // 0xa4(0x01)
	char pad_A4_1 : 7;								  // 0xa4(0x01)
	char pad_A5[0xbb];								  // 0xa5(0xbb)
	struct FUniqueNetIdRepl PlayerId;				  // 0x160(0x28)
	char pad_188[0x48];								  // 0x188(0x48)
	double LastReceiveTime;							  // 0x1d0(0x08)
	char pad_1D8[0x1338];							  // 0x1d8(0x1338)
	struct TArray<struct UChannel*> ChannelsToTick;	  // 0x1510(0x10)
	char pad_1520[0x688];							  // 0x1520(0x688)
};
