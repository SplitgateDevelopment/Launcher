#pragma once

#include "Fwd.h"

struct FTextData
{
	uint8_t Pad[0x28];
	wchar_t* Name;
	int32_t Length;
};
