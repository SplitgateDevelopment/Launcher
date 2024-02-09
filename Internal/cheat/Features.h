#pragma once
#include "UnrealEngine/Engine.h"
#include "../settings/Settings.h"
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
			if (Settings.MISC.DestroyConsole) {
				Settings.MISC.DestroyConsole = false;
				logger->DestroyConsole();
			}
			
			std::string name = Settings.MISC.PlayerName;
			LPCWSTR playerName = std::wstring(name.begin(), name.end()).c_str();

			PlayerController->SetName(playerName);
			PlayerController->FOV(Settings.EXPLOITS.FOV);

			if (Settings.MISC.LoadIntoMap) {
				logger->log("INFO", "Loading into map");
				PlayerController->SwitchLevel(L"Simulation_Alpha");
				Settings.MISC.LoadIntoMap = false;
			};

			if (!isIngame(PlayerController)) {
				SetState("In Menu");
				break;
			};
			SetState("In game");

			APawn* Player = PlayerController->PlayerState->PawnPrivate;
			USkeletalMeshComponent* Mesh = Player->Mesh;

			collision = false;
			if (GetAsyncKeyState(Settings.EXPLOITS.NoClip)) collision = !collision;

			if (collision && Player->GetActorEnableCollision()) Player->SetActorEnableCollision(false);
			else if (!collision && !Player->GetActorEnableCollision()) Player->SetActorEnableCollision(true);
			

			if (Player->Health != 0) {
				UpdatePlayerHealth(Player);
			};

			if (Settings.EXPLOITS.SpinBot && Mesh) {
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

	void UpdatePlayerHealth(APawn* Player) {
		if (Settings.EXPLOITS.GodMode) {
			float health = 999999;
			if (Player->MaxHealth != health) Player->MaxHealth = health;
			if (Player->Health != health) Player->Health = health;

			//Instigator->healthRechargeDelay = 0.1f;

			return;
		}

		if (Player->Health > 100) {
			Player->MaxHealth = 100;
			Player->Health = 100;

			return;
		}

		return;
	}
};