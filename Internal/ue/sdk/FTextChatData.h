#pragma once

#include "Fwd.h"

// ScriptStruct PortalWars.TextChatData
// Size: 0x60 (Inherited: 0x00)
struct FTextChatData
{
	struct FString SenderName;		  // 0x00(0x10)
	struct FUniqueNetIdRepl SenderID; // 0x10(0x28)
	struct FString SenderText;		  // 0x38(0x10)
	struct FString NiceText;		  // 0x48(0x10)
	enum class EChatType ChatType;	  // 0x58(0x01)
	char pad_59[0x7];				  // 0x59(0x07)
};
