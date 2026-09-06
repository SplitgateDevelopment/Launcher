#pragma once

#include "Fwd.h"
#include "Values.h"
#include "UObject.h"

// Class Engine.Font
// Size: 0x1d0 (Inherited: 0x28)
struct UFont : UObject
{
	char pad_28[0x8];								 // 0x28(0x08)
	char FontCacheType[0x01];						 // 0x30(0x01)
	char pad_31[0x7];								 // 0x31(0x07)
	struct TArray<struct FFontCharacter> Characters; // 0x38(0x10)
	struct TArray<struct UTexture2D*> Textures;		 // 0x48(0x10)
	int32_t IsRemapped;								 // 0x58(0x04)
	float EmScale;									 // 0x5c(0x04)
	float Ascent;									 // 0x60(0x04)
	float Descent;									 // 0x64(0x04)
	float Leading;									 // 0x68(0x04)
	int32_t Kerning;								 // 0x6c(0x04)
	char ImportOptions[0xb0];						 // 0x70(0xb0)
	int32_t NumCharacters;							 // 0x120(0x04)
	char pad_124[0x4];								 // 0x124(0x04)
	struct TArray<int32_t> MaxCharHeight;			 // 0x128(0x10)
	float ScalingFactor;							 // 0x138(0x04)
	int32_t LegacyFontSize;							 // 0x13c(0x04)
	struct FName LegacyFontName;					 // 0x140(0x08)
	char CompositeFont[0x38];						 // 0x148(0x38)
	char pad_180[0x50];								 // 0x180(0x50)
};
