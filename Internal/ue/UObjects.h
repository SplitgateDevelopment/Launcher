#pragma once

/// @file
/// @brief Cached UFunction/UObject handles resolved from the UE SDK.
///
/// Declares the core UObject/TArray/FName SDK types' well-known engine and game
/// UFunctions (teleport, draw line/text, get pawn, actor location/rotation,
/// collision, etc.) and the font used by features. The handles are defined in
/// UObjects.cpp and resolved once via Engine::UObjects::Init so hot paths can
/// reuse the pointers instead of looking each up by name every call.
#include "Engine.h"

namespace Engine
{
	namespace UObjects
	{
		extern UObject* SwitchLevelUFunc;
		extern UObject* FOVUFunc;
		extern UObject* SetNameUFunc;
		extern UObject* K2_DrawLineUFunc;
		extern UObject* K2_DrawTextUFunc;
		extern UObject* Font;
		extern UObject* GetSaveGameUFUnc;
		extern UObject* K2_GetPawnUFunc;
		extern UObject* K2_SetRelativeRotationUFunc;
		extern UObject* K2_TeleportTo;
		extern UObject* K2_GetActorLocation;
		extern UObject* K2_GetActorRotation;
		extern UObject* SetActorEnableCollision;
		extern UObject* GetActorEnableCollision;
		extern UObject* K2_SetActorLocation;

		/// Resolve every cached handle above from the global object array. Call once from Engine::Init.
		void Init();
	} // namespace UObjects
} // namespace Engine
