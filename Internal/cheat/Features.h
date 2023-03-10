#pragma once
#include "UnrealEngine/Engine.h"
#include "../menu/Settings.h"
#include "../utils/Logger.h"
#include "../discord/DiscordRPC.h"

class Features {
public:
	Logger* logger = new Logger({
		FALSE,
	});
	DiscordRPC* rpc = new DiscordRPC(logger);

	void handle(APlayerController* PlayerController) {
		do {
			//if (Settings::DestroyConsole) logger->DestroyConsole();
			
			PlayerController->SetName(L"SplitgateDevelopement");
			PlayerController->FOV(Settings::FOV);

			if (Settings::LoadIntoMap) {
				logger->log("INFO", "Loading into map");
				PlayerController->SwitchLevel(L"Simulation_Alpha");
				Settings::LoadIntoMap = false;
			};

			if (!isIngame(PlayerController)) {
				SetState("In Menu");
				break;
			};
			SetState("In game");

			APawn* Player = PlayerController->PlayerState->PawnPrivate;
			USkeletalMeshComponent* Mesh = Player->Mesh;

			collision = false;
			if (GetAsyncKeyState(Settings::NoClip)) collision = !collision;

			if (collision && Player->GetActorEnableCollision()) Player->SetActorEnableCollision(false);
			else if (!collision && !Player->GetActorEnableCollision()) Player->SetActorEnableCollision(true);
			

			if (Settings::GodMode && Player->Health != 0) {
				float health = 999999;
				if (Player->MaxHealth != health) Player->MaxHealth = health;
				if (Player->Health != health) Player->Health = health;

				//Instigator->healthRechargeDelay = 0.1f;
			};

			if (Settings::SpinBot && Mesh) {
				if (spin_yaw > 360.f) spin_yaw = 0.f;

				Mesh->K2_SetRelativeRotation(FRotator(0.f, spin_yaw, 0.f), true, false);
				spin_yaw += 20.f;
			};
		} while (false);

		return;
	};

private:
	bool collision = false;
	float spin_yaw = 0.f;

	bool isIngame(APlayerController* PlayerController) {
		return (PlayerController->AcknowledgedPawn);
	}

	void SetState(const char* state) {
		if (rpc->GetState() == state) return;
		return rpc->UpdateState(state);
	}
};