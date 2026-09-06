#pragma once

#include "Fwd.h"
#include "Values.h"
#include "UField.h"

// Class CoreUObject.Struct
// Size: 0xb0 (Inherited: 0x30)
struct UStruct : UField
{
	char pad_30[0x10];		   // 0x30(0x10)
	UStruct* SuperStruct;	   // 0x40(0x8)
	char UnknownData_48[0x68]; // 0x48(0x80)
};
