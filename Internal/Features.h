#pragma once
#include "Engine.h"

class Features {
public:
	bool GodMode;
	int FovMode = 0;
	bool IncreasedSpeed;
	bool SuperiorMelee;
	bool InfiniteJetpack;
	bool DisableOutOfBounds;
	bool ExtendedSprays;

	int updateFov() {
		if (FovMode == sizeof(fovs)/4 - 1) FovMode = 0;
		else FovMode++;

		return fovs[FovMode];
	}

	void handle(APlayerController* PlayerController) {
		do {
			if (!ExistCharacter(PlayerController)) break;
			APawn* Instigator = PlayerController->AcknowledgedPawn->Instigator;

			if (GodMode && Instigator->Health != 0) {
				float health = 999999;
				if (Instigator->MaxHealth != health) Instigator->MaxHealth = health;
				if (Instigator->Health != health) Instigator->Health = health;

				//Instigator->healthRechargeDelay = 0.1f;
			};

			if (FovMode && FovMode < sizeof(fovs) && FovMode >= 0) PlayerController->FOV(fovs[FovMode]);

			if (IncreasedSpeed); //Instigator->SprintingSpeedModifier = 1.35f;

			if (SuperiorMelee) {
				//Instigator->MeleeRange = 9999;
				//Instigator->TimeBetweenMelee = 0.01;
				//Instigator->MeleeApplyDmgAngle = 360;
				//Instigator->MeleeApplyDmgRange = 9999;
				//Instigator->MeleeApplyDmgRangeXY = 9999;
			}

			if (InfiniteJetpack) {
				//Instigator->thrustAmountPerTick = 0;
				//Instigator->thrusterVelocityThreshhold = 10;
				//Instigator->thrusterRechargeDelay = 0.001;
				//Instigator->thrusterTotalTime = 99999;
			}

			if (DisableOutOfBounds) {
				Instigator->curTimeOutOfBounds = 0;
				Instigator->maxTimeOutOfBounds = 9999;
			}

			if (ExtendedSprays) {
				Instigator->SprayRange = 9999;
				Instigator->SprayLifetime = 9999;
				Instigator->TimeBetweenSprays = 0.01;
			}
		} while (false);

		return;
	};

private:
	int fovs[4] = { 110, 120, 140, 160 };

	bool ExistCharacter(APlayerController* PlayerController) {
		if (PlayerController->AcknowledgedPawn) {
			return (PlayerController->AcknowledgedPawn->Instigator);
		}
	}
};