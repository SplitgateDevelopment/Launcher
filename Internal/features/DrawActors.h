#pragma once

#include "Feature.h"
#include "../utils/Globals.h"

class DrawActors : public Feature
{
private:
	UObject* CharacterClass = 0;

public:
	DrawActors()
	{
		Name = "DrawActors";
		UpdateEnabled();

		Log("Created");
	};

	void UpdateEnabled()
	{
		Enabled = Settings.DEBUG.DrawActors;
	};

	bool Check()
	{
		if (!Initialized) return false;

		if (!Globals::PlayerController) return false;
		if (!Globals::PlayerController->IsInGame()) return false;

		return true;
	};

	void Init()
	{
		CharacterClass = ObjObjects->FindObject("Class PortalWars.PortalWarsCharacter");

		Initialized = (!!CharacterClass);
		Log("Initialized");
	};

	void Destroy()
	{
	};

	void Run()
	{
		// Canvas is constant for the frame — bail once instead of per actor.
		if (!Globals::Canvas) return;

		auto& Levels = Globals::World->Levels;
		for (int l = 0, levelCount = Levels.Num(); l < levelCount; l++) {
			if (!Levels.IsValidIndex(l)) continue;

			ULevel* Level = Levels[l];
			if (!Level) continue;

			DrawLevelActors(Level);
		}
	};

	void DrawLevelActors(ULevel* Level)
	{
		// Reference, not a copy: `auto` here would deep-copy the whole TArray of
		// actor pointers every level, every frame.
		auto& Actors = Level->Actors;

		auto* controller = Globals::PlayerController;
		auto* localPawn = controller->AcknowledgedPawn;

		for (int a = 0, actorCount = Actors.Num(); a < actorCount; a++) {
			if (!Actors.IsValidIndex(a)) continue;

			auto Actor = Actors[a];

			if (!Actor) continue;
			if (!Actor->RootComponent) continue;
			if (!Actor->IsA(CharacterClass)) continue;
			if (Actor == localPawn) continue;

			auto Character = reinterpret_cast<APortalWarsCharacter*>(Actor);

			FVector2D rootPos2D = Character->Mesh->GetBone(BoneFNames::Root, controller);
			if (!rootPos2D.X && !rootPos2D.Y) continue;

			Globals::Canvas->K2_DrawText(0, Actor->GetName(), rootPos2D, { 1.f, 1.f }, { 1.f, 1.f, 1.f, 1.f }, 1.f, { 0.0f, 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f }, false, false, true, { 0.0f, 0.0f, 0.0f, 1.f });
		};
	};
};