#pragma once

#include "Fwd.h"
#include "Values.h"

// Class CoreUObject.Object
// Size: 0x28 (Inherited: 0x00)
struct UObject
{
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

	static struct UObject* GetDefaultObj();
	static struct UClass* StaticClass();
};
