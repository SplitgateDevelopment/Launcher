#pragma once

#include "Fwd.h"

struct FNameEntry
{
	uint16_t bIsWide : 1;
	uint16_t LowercaseProbeHash : 5;
	uint16_t Len : 10;
	union
	{
		char AnsiName[1024];
		wchar_t WideName[1024];
	};

	std::string String();
};
