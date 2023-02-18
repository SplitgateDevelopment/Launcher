#pragma once
#include "UnrealEngine/Engine.h"
#include "../menu/Settings.h"
#include "../utils/Logger.h"

class Features {
public:
	Logger* logger = new Logger({
		FALSE,
	});

	void handle(APlayerController* PlayerController) {
		do {
			PlayerController->SetName(L"SplitgateDevelopement");
			PlayerController->FOV(Settings::FOV);

			if (Settings::LoadIntoMap) {
				PlayerController->SwitchLevel(L"Simulation_Alpha");
				Settings::LoadIntoMap = false;
			};

			if (!ExistCharacter(PlayerController)) break;
			APawn* Player = PlayerController->AcknowledgedPawn->Instigator;

			if (Settings::GodMode && Player->Health != 0) {
				float health = 999999;
				if (Player->MaxHealth != health) Player->MaxHealth = health;
				if (Player->Health != health) Player->Health = health;

				//Instigator->healthRechargeDelay = 0.1f;
			};
		} while (false);

		return;
	};

private:
	bool ExistCharacter(APlayerController* PlayerController) {
		return (PlayerController->AcknowledgedPawn);
	}
};