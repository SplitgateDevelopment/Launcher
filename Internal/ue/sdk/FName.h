#pragma once

#include "Fwd.h"

struct FName
{
	uint32_t Index;
	uint32_t Number;

	std::string GetName();

	inline bool operator==(const FName& Other) const
	{
		return Index == Other.Index && Number == Other.Number;
	}

	inline bool operator!=(const FName& Other) const
	{
		return Index != Other.Index || Number != Other.Number;
	}
};
