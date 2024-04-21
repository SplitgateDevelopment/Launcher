#pragma once

#include "../Feature.h"
#include "../../utils/Globals.h"

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
		UpdateEnabled();

		if (!Initialized) return false;

		if (!Globals::PlayerController) return false;
		if (!Globals::PlayerController->IsInGame()) return false;

		Log(Enabled ? "Enabled" : "Not Enabled");
		return Enabled;
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
		for (auto l = 0; l < Globals::World->Levels.Num(); l++) {
			if (!Globals::World->Levels.IsValidIndex(l)) continue;

			ULevel* Level = Globals::World->Levels[l];
			if (!Level) continue;

			DrawLevelActors(Level);
		}
	};

	void DrawLevelActors(ULevel* Level)
	{
		auto Actors = Level->Actors;
		for (auto a = 0; a < Actors.Num(); a++) {
			if (!Actors.IsValidIndex(a)) continue;

			auto Actor = Actors[a];

			if (!Actor) continue;
			if (!Actor->RootComponent) continue;
			if (!Actor->IsA(CharacterClass)) continue;
			if (Actor == Globals::PlayerController->AcknowledgedPawn) continue;

			auto Character = reinterpret_cast<APortalWarsCharacter*>(Actor);
			auto Mesh = Character->Mesh;

			FVector2D rootPos2D = Mesh->GetBone(BoneFNames::Root, Globals::PlayerController);
			if (!rootPos2D.X && !rootPos2D.Y) continue;

			if (!Globals::Canvas) continue;
			Globals::Canvas->K2_DrawText(0, Actor->GetName(), rootPos2D, { 1.f, 1.f }, { 1.f, 1.f, 1.f, 1.f }, 1.f, { 0.0f, 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f }, false, false, true, { 0.0f, 0.0f, 0.0f, 1.f });
		};
	};
};