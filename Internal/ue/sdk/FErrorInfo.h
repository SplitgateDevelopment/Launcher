#pragma once

#include "Fwd.h"

// ScriptStruct PortalWarsGlobals.ErrorInfo
// Size: 0x58 (Inherited: 0x00)
struct FErrorInfo
{
	struct FString ErrorCode;					// 0x00(0x10)
	struct TArray<struct FErrorData> ErrorData; // 0x10(0x10)
	struct FText ErrorText;						// 0x20(0x18)
	struct FString Code;						// 0x38(0x10)
	struct FString Message;						// 0x48(0x10)
};
