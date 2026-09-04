#pragma once

/// @file
/// The DebugNames feature: draws the UObject name of every actor in the world at
/// its screen position. Independent of the ESP — a debug aid for discovering
/// classes/objects. Heavy (one projection + text draw per actor), so it is
/// opt-in via Settings.VISUALS.DrawAllNames.

#include "Feature.h"
#include "../utils/Globals.h"
#include "../render/Render.h"

#include <string>

/// Draws every actor's UObject name each frame while enabled (debug only).
class DebugNames : public Feature
{
  public:
	DebugNames()
	{
		Name = "DebugNames";
		UpdateEnabled();

		Log("Created");
	};

	void UpdateEnabled()
	{
		Enabled = Settings.VISUALS.DrawAllNames;
	};

	bool Check()
	{
		if (!Initialized) return false;
		if (!Globals::PlayerController) return false;
		if (!Globals::PlayerController->IsInGame()) return false;
		if (!Globals::Canvas) return false;

		return true;
	};

	/// No game object to resolve; ready immediately.
	void Init()
	{
		Initialized = true;
		Log("Initialized");
	};

	void Destroy() {
	};

	void Run()
	{
		auto* controller = Globals::PlayerController;
		const FLinearColor color{1.f, 1.f, 1.f, 1.f};
		const float scale = Settings.VISUALS.FontScale;

		auto& Levels = Globals::World->Levels;
		for (int l = 0, levelCount = Levels.Num(); l < levelCount; l++)
		{
			if (!Levels.IsValidIndex(l)) continue;

			ULevel* Level = Levels[l];
			if (!Level) continue;

			auto& Actors = Level->Actors;
			for (int a = 0, actorCount = Actors.Num(); a < actorCount; a++)
			{
				if (!Actors.IsValidIndex(a)) continue;

				auto Actor = Actors[a];
				if (!Actor || !Actor->RootComponent) continue;

				FVector2D screen;
				if (!controller->ProjectWorldLocationToScreen(Actor->K2_GetActorLocation(), screen, false)) continue;
				if (!screen.X && !screen.Y) continue;

				Render::Text(screen, Actor->GetName(), scale, color);
			}
		}
	};
};
